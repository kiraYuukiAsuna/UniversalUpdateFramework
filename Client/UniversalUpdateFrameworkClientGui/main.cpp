#include <QApplication>
#include <QPushButton>
#include <QFontDatabase>
#include "src/ui/MainWindow.h"

int main(int argc, char *argv[]) {
    setbuf(stdout, nullptr);

    QApplication a(argc, argv);

    const QString fontPath = QString(R"(:/fonts/SourceHanSansCN/SourceHanSansCN-Regular.ttf)");
    const int loadedFontID = QFontDatabase::addApplicationFont(fontPath);
    if (const QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
            !loadedFontFamilies.empty()) {
        const QString&sansCNFamily = loadedFontFamilies.at(0);
        QFont defaultFont = QApplication::font();
        defaultFont.setFamily(sansCNFamily);
        defaultFont.setPixelSize(14);
        QApplication::setFont(defaultFont);
    }

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
