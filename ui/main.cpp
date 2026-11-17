#include "ui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ui window;
    window.show();
    return app.exec();
}
