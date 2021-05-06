#include "soprocessor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Soprocessor wgt;
    wgt.setWindowTitle("Soprocessor EE");
    wgt.setWindowIcon(QIcon("icon.png"));
    wgt.resize(250,300);
    wgt.move(700,400);
    wgt.show();
    return a.exec();
}
