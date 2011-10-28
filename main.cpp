#include <QtGui/QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include "ImageCropper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget window;


    ImageCropper *w = new ImageCropper(&window);
    w->setProportion(QSize(4,3));
    w->setPixmap(QPixmap("PATH_TO_IMAGE"));

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(w);
    QCheckBox *cb = new QCheckBox(&window);
    QObject::connect(cb, SIGNAL(toggled(bool)),
                     w, SLOT(setIsProportionFixed(bool)));
    cb->setText("Fix proportion to (4x3)");
    layout->addWidget(cb);
    window.setLayout(layout);

    window.show();

    return a.exec();
}
