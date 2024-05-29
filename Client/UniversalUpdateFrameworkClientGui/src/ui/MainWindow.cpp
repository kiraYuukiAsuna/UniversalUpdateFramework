#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "src/framework/Definition/ImageDefination.hpp"
#include <QMessageBox>
#include "UpdateLogic/UpdateClientAppEntry.hpp"
#include <QProcess>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowIcon(QIcon(Image::ImageAppIcon));

    initialize();

    connect(ui->Refresh, &QPushButton::clicked, this, [&]() {
        refresh();
    });

    connect(ui->LaunchBtn, &QPushButton::clicked, this, [&]() {
        QList<QString> arg;
        QProcess process(this);
        connect(&process, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
            qDebug() << "Error occurred: " << error;
        });

        auto executableFileWorkingDir =
                QApplication::applicationDirPath() + "/" + QString::fromStdString(m_AppSpecification.appExecutablePath);
        auto executableFileFullPath =
                executableFileWorkingDir + "/" + QString::fromStdString(m_AppSpecification.appExecutableName);
        qDebug() << executableFileFullPath;
        process.setWorkingDirectory(executableFileWorkingDir);
        process.startDetached(executableFileFullPath, arg);
    });

    connect(ui->UpdateBtn, &QPushButton::clicked, this, [&]() {
        refresh();

        if (m_ServerCurrentAppVersion.AppVersion.empty()) {
            QMessageBox::critical(this, "Error", "Cannot connect to update server!");
            return;
        }

        UpdateMode updateMode{UpdateMode::Unknown};

        switch (ui->UpdateModeSelect->currentIndex()) {
            case 0: {
                updateMode = UpdateMode::MultiVersionDifferencePackageUpdate;
                break;
            }
            case 1: {
                updateMode = UpdateMode::DifferenceUpdate;
                break;
            }
            case 2: {
                updateMode = UpdateMode::FullPackageUpdate;
                break;
            }
            default: {
                updateMode = UpdateMode::DifferenceUpdate;
                break;
            }
        }

        UpdateConfigIo updateConfigIo(m_AppSpecification.appUpdateConfigFile);
        updateConfigIo.readFromFile();

        auto result = std::async(std::launch::async,
                                 handleUpdateMode,
                                 std::ref(updateMode),
                                 std::ref(updateConfigIo),
                                 m_ServerCurrentAppVersion.AppVersion);

        ui->UpdateProgressBar->setValue(10);

        auto updateResult = result.get();
        if (updateResult.getStatus()) {
            ui->UpdateProgressBar->setValue(100);
            QMessageBox::information(this, "Info", "Update finished!");
        }
        else {
            ui->UpdateProgressBar->setValue(0);
            QMessageBox::critical(this, "Info",
                                  "Update failed!" + QString::fromStdString(updateResult.getErrorMessage()));
        }

        refresh();
        m_AppSpecificationIo.writeToFile();
    });


    refresh();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::refresh() {
    m_AppSpecificationIo.readFromFile();
    m_AppSpecification = m_AppSpecificationIo.getConfig();

    UpdateConfigIo updateConfigIo(m_AppSpecification.appUpdateConfigFile);
    updateConfigIo.readFromFile();
    m_UpdateConfig = updateConfigIo.getConfig();

    ui->AppSelect->clear();
    ui->AppSelect->addItem(QString::fromStdString(m_UpdateConfig.appName));

    if (m_UpdateConfig.localCurrentVersion == "x.x.x" || m_UpdateConfig.localCurrentVersion.empty()) {
        m_UpdateConfig.localCurrentVersion = "0.0.0";
        updateConfigIo.setConfig(m_UpdateConfig);
        updateConfigIo.writeToFile();
    }
    auto localCurrentVersion = Version(m_UpdateConfig.localCurrentVersion);

    ui->LocalCurrentVersion->setText(QString::fromStdString(localCurrentVersion.getVersionString()));

    try {
        if (auto [result, appVersionContent] = async_simple::coro::syncAwait(m_ApiRequest->GetCurrentAppVersion()); result.getStatus()) {
            m_ServerCurrentAppVersion = AppVersionInfo(nlohmann::json::parse(appVersionContent));
            auto serverCurrentVersion = Version{m_ServerCurrentAppVersion.AppVersion};

            ui->ServerCurrentVersion->setText(
                QString::fromStdString(m_ServerCurrentAppVersion.AppVersion));

            if (localCurrentVersion < serverCurrentVersion) {
                ui->UpdateStatus->setText("New Version Available!");
                if (auto [result, appManifestContent] = async_simple::coro::syncAwait(m_ApiRequest->GetCurrentAppManifest()); result.getStatus()) {
                    AppManifestInfo appManifest = nlohmann::json::parse(appManifestContent);
                    ui->textEdit->setMarkdown(QString::fromStdString(appManifest.UpdateReadMe));
                }
            }
            else {
                ui->UpdateStatus->setText("No Update Available!");
                ui->textEdit->setMarkdown(QString::fromStdString("# No Update Available!"));
            }
        }
        else {
            QMessageBox::critical(this, "Error", QString::fromLocal8Bit(result.getErrorMessage()));
            ui->UpdateStatus->setText("Network Error!");
        }
    }catch (std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        ui->UpdateStatus->setText("Network Error!");
    }
}

void MainWindow::initialize() {
    ui->UpdateModeSelect->clear();
    ui->UpdateModeSelect->addItem("MultiVersionDifferencePackageUpdate");
    ui->UpdateModeSelect->addItem("DirectDifferenceUpdate");
    ui->UpdateModeSelect->addItem("FullPackageUpdate");

    UpdateConfigIo updateConfigIo(m_AppSpecification.appUpdateConfigFile);
    updateConfigIo.readFromFile();
    m_UpdateConfig = updateConfigIo.getConfig();

    if (m_UpdateConfig.localCurrentVersion == "x.x.x" || m_UpdateConfig.localCurrentVersion.empty()) {
        m_UpdateConfig.localCurrentVersion = "0.0.0";
        updateConfigIo.setConfig(m_UpdateConfig);
        updateConfigIo.writeToFile();
    }

    m_ApiRequest = new ApiRequest(m_UpdateConfig.host, m_UpdateConfig.appName, m_UpdateConfig.channel,
                                  m_UpdateConfig.platform);
}
