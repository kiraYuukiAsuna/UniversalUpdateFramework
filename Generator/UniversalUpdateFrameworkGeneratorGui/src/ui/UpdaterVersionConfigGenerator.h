#pragma once

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class UpdaterVersionConfigGenerator; }
QT_END_NAMESPACE

class UpdaterVersionConfigGenerator : public QDialog {
Q_OBJECT

public:
    explicit UpdaterVersionConfigGenerator(QWidget *parent = nullptr);
    ~UpdaterVersionConfigGenerator() override;

private:
    Ui::UpdaterVersionConfigGenerator *ui;
};
