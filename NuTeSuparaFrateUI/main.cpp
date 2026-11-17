#include "NuTeSuparaFrateUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NuTeSuparaFrateUI window;
    window.show();
    return app.exec();
}
