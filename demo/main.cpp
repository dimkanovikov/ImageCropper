#include <QApplication>
#include "demowidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	DemoWidget window;
    window.show();

    return a.exec();
}
