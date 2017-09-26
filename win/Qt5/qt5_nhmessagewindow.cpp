#include "qt5_port.h"

#include <QVBoxLayout>
#include <QLabel>

NHMessageWindow::NHMessageWindow(QWidget *parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setTitleBarWidget(new QWidget());
    setMinimumHeight(50);

    content = new QTextEdit(this);
    content->setReadOnly(true);
    content->setFocusPolicy(Qt::NoFocus);

    setWidget(content);
}

void NHMessageWindow::print_line(const QString line)
{
    print_string(line + "\n");
}

void NHMessageWindow::print_string(const QString str)
{
    QTextCursor cursor = content->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(str);
    content->ensureCursorVisible();
}

void NHMessageWindow::print_yn(const char *ques, const char *choices, int dflt)
{
    QString question;
    QTextStream(&question) << ques << "[" << choices << "][" << (char) dflt << "]";
    print_string(question);
}
