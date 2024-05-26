#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "src/Defination.hpp"
#include "src/GeneratorAppEntry.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->GenerationMode->addItem("FullPackage");
    ui->GenerationMode->addItem("DifferencePackage");

    connect(ui->SelectOldVersionPath, &QPushButton::clicked, this, [&]() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);
        auto selectedPath = dialog.getExistingDirectory(this, "Select Old Version Path", QStandardPaths::displayName(
                QStandardPaths::StandardLocation::HomeLocation), QFileDialog::Option::ShowDirsOnly);

        ui->OldVersionPath->setText(selectedPath);
    });

    connect(ui->SelectNewVersionPath, &QPushButton::clicked, this, [&]() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);
        auto selectedPath = dialog.getExistingDirectory(this, "Select New Version Path", QStandardPaths::displayName(
                QStandardPaths::StandardLocation::HomeLocation), QFileDialog::Option::ShowDirsOnly);

        ui->NewVersionPath->setText(selectedPath);
    });

    connect(ui->CreateAppPackage, &QPushButton::clicked, this, [&]() {
        if (ui->Channel->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "You need to specify Channel!");
            return;
        }

        if (ui->Platform->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "You need to specify Platform!");
            return;
        }

        if (ui->GenerationMode->currentIndex() == 0) {
            if (ui->NewVersionPath->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to select New version path!");
                return;
            }

            if (ui->AppName->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to specify the App Name!");
                return;
            }

            if (ui->AppVersion->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to specify App Version!");
                return;
            }

            std::filesystem::path newPath(ui->NewVersionPath->text().toStdString());
            if (!std::filesystem::exists(newPath)) {
                QMessageBox::critical(this, "Error", "New Version Path not exist!");
                return;
            }

            UpdatePackageBuildInfo packageBuildInfo;
            packageBuildInfo.PackageMode = "FullPackage";
            packageBuildInfo.NewVersionPath = ui->NewVersionPath->text().toStdString();
            packageBuildInfo.AppName = ui->AppName->text().toStdString();
            packageBuildInfo.Channel = ui->Channel->text().toStdString();
            packageBuildInfo.Platform = ui->Platform->text().toStdString();
            packageBuildInfo.AppCurrentVersion = ui->AppVersion->text().toStdString();

//            packageBuildInfo.PackageMode = "DifferencePackage";
//            packageBuildInfo.newPath = R"(D:\WorkSpace\UnrealEngine\1.1.0)";
//            packageBuildInfo.appname = "TestApplication";
//            packageBuildInfo.channel = "Release";
//            packageBuildInfo.platform = "Win";
//            packageBuildInfo.appversion = "1.1.0";

            if (generateFullPackage(packageBuildInfo) != 0) {
                QMessageBox::critical(this, "Error", "Generate FullPackage Failed!");
                return;
            } else {
                QMessageBox::information(this, "Info", "Generate FullPackage Successfully!");
                return;
            }

        } else if (ui->GenerationMode->currentIndex() == 1) {
            if (ui->OldVersionPath->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to select Old version path!");
                return;
            }

            if (ui->NewVersionPath->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to select New version path!");
                return;
            }

            if (ui->AppName->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to specify the App Name!");
                return;
            }

            if (ui->AppVersion->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to specify App Version!");
                return;
            }

            if (ui->OldAppVersion->text().isEmpty()) {
                QMessageBox::critical(this, "Error", "You need to specify Old App Version!");
                return;
            }

            std::filesystem::path oldPath(ui->OldVersionPath->text().toStdString());
            if (!std::filesystem::exists(oldPath)) {
                QMessageBox::critical(this, "Error", "Old Version Path not exist!");
                return;
            }

            std::filesystem::path newPath(ui->NewVersionPath->text().toStdString());
            if (!std::filesystem::exists(newPath)) {
                QMessageBox::critical(this, "Error", "New Version Path not exist!");
                return;
            }

            UpdatePackageBuildInfo packageBuildInfo;
            packageBuildInfo.PackageMode = "DifferencePackage";
            packageBuildInfo.OldVersionPath = ui->OldVersionPath->text().toStdString();
            packageBuildInfo.NewVersionPath = ui->NewVersionPath->text().toStdString();
            packageBuildInfo.AppName = ui->AppName->text().toStdString();
            packageBuildInfo.Channel = ui->Channel->text().toStdString();
            packageBuildInfo.Platform = ui->Platform->text().toStdString();
            packageBuildInfo.AppCurrentVersion = ui->AppVersion->text().toStdString();
            packageBuildInfo.AppBeforeVersion = ui->OldAppVersion->text().toStdString();

//            packageBuildInfo.PackageMode = "DifferencePackage";
//            packageBuildInfo.oldPath = R"(C:/Users/KiraY/Desktop/UniversalUpdateFramework/Generator/UniversalUpdateFrameworkGeneratorGui/cmake-build-debug/TestApplication/Release/Win/1.0.0/fullpackage)";
//            packageBuildInfo.newPath = R"(C:/Users/KiraY/Desktop/UniversalUpdateFramework/Generator/UniversalUpdateFrameworkGeneratorGui/cmake-build-debug/TestApplication/Release/Win/1.1.0/fullpackage)";
//            packageBuildInfo.appname = "TestApplication";
//            packageBuildInfo.channel = "Release";
//            packageBuildInfo.platform = "Win";
//            packageBuildInfo.appversion = "1.1.0";
//            packageBuildInfo.appbeforeversion = "1.0.0";

            if (generateDifferencePackage(packageBuildInfo) != 0) {
                QMessageBox::critical(this, "Error", "Generate DifferencePackage Failed!");
                return;
            } else {
                QMessageBox::information(this, "Info", "Generate DifferencePackage Successfully!");
                return;
            }
        }

    });
}

MainWindow::~MainWindow() {
    delete ui;
}
