

#include "stdafx.h"
#include "MainWidget.h"

int main(int argc, char** argv) {
    
    QApplication a(argc,argv);
    MainWidget w;
    w.resize(300, 300);
    w.show();
    a.exec();
    
    return 0;
}
