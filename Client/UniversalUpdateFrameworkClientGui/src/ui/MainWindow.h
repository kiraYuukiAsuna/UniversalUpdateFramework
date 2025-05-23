#pragma once

#include <Defination.hpp>
#include <QMainWindow>
#include <QStandardPaths>
#include <TypeDefination/AppVersion.hpp>

#include "Network/ApiRequest.hpp"
#include "UpdateCore/UpdateConfig.hpp"
#include "src/framework/Definition/AppSpecification.hpp"

struct UpdateStatusInfo;
QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow() override;

private:
	Ui::MainWindow *ui;

	AppSpecificationIo m_AppSpecificationIo{"AppSpecification.json"};

	UpdateConfig m_UpdateConfig;
	AppSpecification m_AppSpecification;
	ApiRequest *m_ApiRequest;
	AppVersionInfo m_ServerCurrentAppVersion;

	std::future<ReturnWrapper> m_UpdateFuture;

	void refresh();

	void initialize();

	void handleUpdateStatusInfo(UpdateStatusInfo updateStatusInfo);

	std::string getTempLocation();

	void executeUpdate(UpdateMode mode);
};
