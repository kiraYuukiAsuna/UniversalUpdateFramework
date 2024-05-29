#include <Log.h>
#include <QApplication>
#include <QFontDatabase>
#include <QProcess>
#include <util.hpp>

#include "src/ui/MainWindow.h"

int main(int argc, char *argv[]) {
    setbuf(stdout, nullptr);

    Seele::Log::Init("logs", "Core.log", "App.log", "EditorConsole.log", true, false);
    auto timestamp = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).
        time_since_epoch();
    SEELE_INFO_TAG("Main", "App Start At: {}", util::timestampToString(timestamp.count()));

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

    auto exitCode = QApplication::exec();
    if (exitCode == 888) {
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    }

    Seele::Log::Shutdown();

    return exitCode;
}
