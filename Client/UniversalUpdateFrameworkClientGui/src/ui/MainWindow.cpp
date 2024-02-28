#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "src/framework/Definition/ImageDefination.hpp"
#include "UpdateCore/AppVersion.hpp"
#include <QMessageBox>
#include "UpdateLogic/UpdateClientAppEntry.hpp"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
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
                executableFileWorkingDir + "/" + QString::fromStdString(m_AppSpecification.appExecutableName) + ".exe";
        qDebug() << executableFileFullPath;
        process.setWorkingDirectory(executableFileWorkingDir);
        process.startDetached(executableFileFullPath, arg);
    });

    connect(ui->UpdateBtn, &QPushButton::clicked, this, [&]() {
        refresh();

        if (m_ServerCurrentAppVersion == nullptr) {
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
                                 m_ServerCurrentAppVersion->getVersion().getVersionString());

        ui->UpdateProgressBar->setValue(10);

        result.get();

        ui->UpdateProgressBar->setValue(100);

        QMessageBox::information(this, "Info", "Update finished!");

        refresh();
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

    if (m_UpdateConfig.localCurrentVersion == "x.x.x") {
        m_UpdateConfig.localCurrentVersion = "0.0.0";
        updateConfigIo.setConfig(m_UpdateConfig);
        updateConfigIo.writeToFile();
    }
    auto localCurrentVersion = Version(m_UpdateConfig.localCurrentVersion);

    ui->LocalCurrentVersion->setText(QString::fromStdString(localCurrentVersion.getVersionString()));

    m_ApiRequest = new ApiRequest(m_UpdateConfig.host, m_UpdateConfig.appName, m_UpdateConfig.channel,
                                  m_UpdateConfig.platform);
    if (auto [result, appVersionContent] = m_ApiRequest->GetCurrentAppVersion();result.getStatus()) {
        m_ServerCurrentAppVersion = new AppVersion(nlohmann::json::parse(appVersionContent));
        auto serverCurrentVersion = m_ServerCurrentAppVersion->getVersion();

        ui->ServerCurrentVersion->setText(
                QString::fromStdString(m_ServerCurrentAppVersion->getVersion().getVersionString()));

        if (localCurrentVersion < serverCurrentVersion) {
            ui->UpdateStatus->setText("New Version Available!");
        } else {
            ui->UpdateStatus->setText("No Update Available!");
        }

    } else {
        QMessageBox::critical(this, "Error", QString::fromStdString(result.getErrorMessage()));
        ui->UpdateStatus->setText("Network Error!");
    }
}

void MainWindow::initialize() {
    ui->UpdateModeSelect->clear();
    ui->UpdateModeSelect->addItem("MultiVersionDifferencePackageUpdate");
    ui->UpdateModeSelect->addItem("DirectDifferenceUpdate");
    ui->UpdateModeSelect->addItem("FullPackageUpdate");

}
