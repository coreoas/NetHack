#include "qt5_port.h"

#include <QVBoxLayout>
#include <QLabel>

NHMessageWindow::NHMessageWindow(QWidget *parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setTitleBarWidget(new QWidget());
    setMinimumHeight(50);
}
