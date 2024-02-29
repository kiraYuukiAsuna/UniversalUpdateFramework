#include <QApplication>
#include "src/ui/MainWindow.h"
#include "src/FullPackageGenerator.hpp"
#include "src/DifferencePackageGenerator.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
