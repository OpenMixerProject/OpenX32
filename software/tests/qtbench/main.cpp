#include "benchmarkwidget.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("qtbench");
    QApplication::setApplicationDisplayName("OpenX32 Qt Benchmark");

    bool windowed = false;
    for (int i = 1; i < argc; ++i) {
        if (QString::fromLocal8Bit(argv[i]) == "--windowed") {
            windowed = true;
        }
    }

    BenchmarkWidget widget;
    widget.setWindowTitle("OpenX32 Qt Benchmark");
    widget.resize(800, 480);

    if (windowed) {
        widget.show();
    } else {
        widget.showFullScreen();
    }

    return app.exec();
}
