#include "qt5_port.h"

NHApplication* NHApplication::_instance = nullptr;

void NHApplication::instantiate(int &argc, char **argv)
{
    if (!_instance) {
        _instance = new NHApplication(argc, argv);
    }
}

NHApplication* NHApplication::get_instance()
{
    return _instance;
}

NHApplication::NHApplication(int &argc, char **argv) : QApplication(argc,argv)
{
}
