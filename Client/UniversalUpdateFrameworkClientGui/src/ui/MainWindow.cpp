#include "MainWindow.h"

#include <Log.h>

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
                updateMode = UpdateMode::MultiVersionDifferencePackageUpdate; {
                    break;
                }
            }
            case 1: {
                updateMode = UpdateMode::DifferenceUpdate; {
                    break;
                }
            }
            case 2: {
                updateMode = UpdateMode::FullPackageUpdate; {
                    break;
                }
            }
            default: {
                updateMode = UpdateMode::DifferenceUpdate; {
                    break;
                }
            }
        }

        int result = QMessageBox::information(this, "Info",
                                              "Are you sure to start update? Please close your application and save your data before update, or you may lose your data and update may fail to work!",
                                              QMessageBox::Yes,
                                              QMessageBox::No);
        if (result != QMessageBox::Yes) {
            return;
        }

        executeUpdate(updateMode);
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
        if (auto [result, appVersionContent] = async_simple::coro::syncAwait(m_ApiRequest->GetCurrentAppVersion());
            result.getStatus()) {
            m_ServerCurrentAppVersion = AppVersionInfo(nlohmann::json::parse(appVersionContent));
            auto serverCurrentVersion = Version{m_ServerCurrentAppVersion.AppVersion};

            ui->ServerCurrentVersion->setText(
                QString::fromStdString(m_ServerCurrentAppVersion.AppVersion));

            if (localCurrentVersion < serverCurrentVersion) {
                ui->UpdateStatus->setText("New Version Available!");
                if (auto [result, appManifestContent] = async_simple::coro::syncAwait(
                    m_ApiRequest->GetCurrentAppManifest()); result.getStatus()) {
                    AppManifestInfo appManifest = nlohmann::json::parse(appManifestContent);
                    ui->UpdateReadMe->setMarkdown(QString::fromStdString(appManifest.UpdateReadMe));
                }
                if(m_ServerCurrentAppVersion.ForceUpdate) {
                    executeUpdate(UpdateMode::MultiVersionDifferencePackageUpdate);
                }
            }
            else {
                ui->UpdateStatus->setText("No Update Available!");
                ui->UpdateReadMe->setMarkdown(QString::fromStdString("# No Update Available!"));
            }
        }
        else {
            QMessageBox::critical(this, "Error", QString::fromLocal8Bit(result.getErrorMessage()));
            ui->UpdateStatus->setText("Network Error!");
        }
    }
    catch (std::exception&e) {
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

void MainWindow::handleUpdateStatusInfo(UpdateStatusInfo updateStatusInfo) {
    // auto* th = new std::thread([&]() {
    //     while (true) {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         QMetaObject::invokeMethod(this, [this]() {
    //             ui->UpdateLog->setText("Hello form thread");
    //             qDebug() << "Hello from thread";
    //         }, Qt::QueuedConnection);
    //     }
    // });
    // th->detach();
    QMetaObject::invokeMethod(this, [this, updateStatusInfo]() {
        auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).
                time_since_epoch();
        util::timestampToString(timestamp.count());
        ui->UpdateLog->append(QString::fromStdString(util::timestampToString(timestamp.count())));
        static int progress = 0;
        progress++;
        if (progress > 100) {
            progress = 0;
        }
        ui->UpdateProgressBar->setValue(progress);
        auto statusName = std::string{magic_enum::enum_name(updateStatusInfo.status)};
        switch (updateStatusInfo.status) {
            case UpdateStatus::None: { break; }
            case UpdateStatus::DownloadAppVersionFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DownloadAppManifestFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DownloadFullPackageManifestFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DownloadFullPackageFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DownloadDifferencePackageManifestFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DownloadDifferencePackageFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::FullPackageUpdateInstalling: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DifferencePackageUpdateInstalling: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::DifferenceUpdateInstalling: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::MutilVersionUpdateInstalling: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                break;
            }
            case UpdateStatus::VerifyingUpdate: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::RepatchingUpdate: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::VerifyingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::DownloadingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                ui->DownloadFileProgress->setValue(
                    int((float)updateStatusInfo.DownloadCurrentSize / updateStatusInfo.DownloadTotalSize * 100));
                ui->DownloadFileSizeProgress->setText(QString::fromStdString(
                    std::to_string(updateStatusInfo.DownloadCurrentSize / 1024 / 1024) + " / " + std::to_string(
                        updateStatusInfo.DownloadTotalSize / 1024 / 1024) + " MB"));
                break;
            }
            case UpdateStatus::CopyingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::DeletingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::UpdatingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::CreatingFile: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.CurrentFileName));
                break;
            }
            case UpdateStatus::Warning: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.WarningMessage));
                break;
            }
            case UpdateStatus::Completed: {
                ui->UpdateLog->append(QString::fromStdString(statusName));
                auto future = m_UpdateFuture.get();
                m_AppSpecificationIo.writeToFile();
                this->setEnabled(true);
                if (future.getStatus()) {
                    ui->UpdateProgressBar->setValue(100);
                    ui->DownloadFileProgress->setValue(100);
                    progress = 100;
                    QMessageBox::information(this, "Info", "Update finished!");
                }
                else {
                    ui->UpdateProgressBar->setValue(0);
                    ui->DownloadFileProgress->setValue(0);
                    progress = 0;
                    QMessageBox::critical(this, "Info",
                                          "Update failed!" + QString::fromStdString(future.getErrorMessage()));
                }
                refresh();
                break;
            }
            case UpdateStatus::Failed: {
                ui->UpdateLog->append(QString::fromStdString(statusName + ":\n\t" + updateStatusInfo.ErrorMessage));
                this->setEnabled(true);
                ui->UpdateProgressBar->setValue(0);
                ui->DownloadFileProgress->setValue(0);
                QMessageBox::critical(this, "Info",
                                      "Update failed!" + QString::fromStdString(updateStatusInfo.ErrorMessage));
                break;
            }
        }
        ui->UpdateLog->moveCursor(QTextCursor::End);
    }, Qt::QueuedConnection);
}

std::string MainWindow::getTempLocation() {
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation).toStdString();
}

void MainWindow::executeUpdate(UpdateMode mode) {
    UpdateConfigIo updateConfigIo(m_AppSpecification.appUpdateConfigFile);
    updateConfigIo.readFromFile();

    ui->UpdateProgressBar->setValue(0);
    ui->DownloadFileProgress->setValue(0);
    ui->UpdateLog->clear();

    this->setEnabled(false);
    m_UpdateFuture = std::async(std::launch::async,
                                handleUpdateMode,
                                mode,
                                updateConfigIo,
                                m_ServerCurrentAppVersion.AppVersion, [this](UpdateStatusInfo updateStatusInfo) {
                                    handleUpdateStatusInfo(updateStatusInfo);
                                });
}
