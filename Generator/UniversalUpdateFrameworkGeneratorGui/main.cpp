#include <QApplication>
#include "src/ui/MainWindow.h"
#include "FullPackageGenerator.hpp"
#include "DifferencePackageGenerator.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
