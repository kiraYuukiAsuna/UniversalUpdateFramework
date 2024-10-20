#include <QApplication>

#include "DifferencePackageGenerator.hpp"
#include "FullPackageGenerator.hpp"
#include "src/ui/MainWindow.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	MainWindow mainWindow;
	mainWindow.show();

	return QApplication::exec();
}
