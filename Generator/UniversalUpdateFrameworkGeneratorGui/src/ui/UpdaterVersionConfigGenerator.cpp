#include "UpdaterVersionConfigGenerator.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "UpdaterVersionInfoGenerator.hpp"
#include "Version.hpp"
#include "ui_UpdaterVersionConfigGenerator.h"

UpdaterVersionConfigGenerator::UpdaterVersionConfigGenerator(QWidget* parent)
	: QDialog(parent), ui(new Ui::UpdaterVersionConfigGenerator) {
	ui->setupUi(this);

	connect(ui->SelectBtn, &QPushButton::clicked, this, [&]() {
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::Directory);
		auto selectedPath = dialog.getExistingDirectory(
			this, "Select New Version Path",
			QStandardPaths::displayName(
				QStandardPaths::StandardLocation::HomeLocation),
			QFileDialog::Option::ShowDirsOnly);

		ui->lineEdit->setText(selectedPath);
	});

	connect(ui->Generate, &QPushButton::clicked, this, [&]() {
		if (!std::filesystem::exists(ui->lineEdit->text().toStdString())) {
			QMessageBox::critical(this, "Error", "Updater Path Not Exist!");
			return;
		}

		if (ui->lineEdit_2->text().isEmpty()) {
			QMessageBox::critical(this, "Error", "Version Is Empty!");
			return;
		}

		try {
			Version version(ui->lineEdit_2->text().toStdString());

			auto appVersionBriefInfo = GenereateUpdaterVersionInfo(
				version.getVersionString(), ui->lineEdit->text().toStdString());
			nlohmann::json content = appVersionBriefInfo;

			ui->textEdit->setText(QString::fromStdString(content.dump(4)));
		} catch (std::exception& e) {
			QMessageBox::critical(this, "Error", e.what());
		}
	});
}

UpdaterVersionConfigGenerator::~UpdaterVersionConfigGenerator() { delete ui; }
