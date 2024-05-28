#pragma once

#include <QMainWindow>
#include "src/framework/Definition/AppSpecification.hpp"
#include "UpdateCore/UpdateConfig.hpp"
#include "Network/ApiRequest.hpp"
#include <AppVersion.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    ApiRequest* m_ApiRequest;
    AppVersionInfo m_ServerCurrentAppVersion;

    void refresh();

    void initialize();
};
