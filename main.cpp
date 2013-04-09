#include "main.h"
#include "program.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Program wgt;

    wgt.show();
    //wgt.slotDrawGraph();

    return app.exec();
}
