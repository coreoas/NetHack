#include "qt5_port.h"

NHApplication* NHApplication::instance = nullptr;

void NHApplication::instantiate(int &argc, char **argv)
{
    if (!instance) {
        instance = new NHApplication(argc, argv);
    }
}

NHApplication::NHApplication(int &argc, char **argv) : QApplication(argc,argv)
{
    printf("loading application...");

    main_window = new QMainWindow();
    main_window->resize(320, 240);
    main_window->show();

    printf("done\n");
}
