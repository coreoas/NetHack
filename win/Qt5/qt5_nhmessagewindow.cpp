#include "qt5_port.h"

#include <QVBoxLayout>

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
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    content->setTextCursor(cursor);
    cursor.insertText(str, format);
    content->ensureCursorVisible();
}

void NHMessageWindow::set_bold()
{
    format.setFontWeight(QFont::Bold);
}

void NHMessageWindow::unset_bold()
{
    format.setFontWeight(QFont::Normal);
}

void NHMessageWindow::print_yn(const char *ques, const char *choices, int dflt)
{
    QString question;
    QTextStream(&question) << ques << "[" << choices << "][" << (char) dflt << "]";
    print_string(question);
}

void NHMessageWindow::remove_chars(const int n)
{
    QTextCursor cursor = content->textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    content->setTextCursor(cursor);
    int i;
    for (i = 0; i < n; i++) {
        content->textCursor().deletePreviousChar();
    }
}
