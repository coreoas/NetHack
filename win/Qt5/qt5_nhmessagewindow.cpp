#include <QScrollBar>
#include <QFontMetrics>
#include "qt5_port.h"

NHMessageWindow::NHMessageWindow(QWidget *parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setTitleBarWidget(new QWidget());

    content = new QTextEdit(this);
    content->setReadOnly(true);
    content->setFocusPolicy(Qt::NoFocus);

    // minimum height is 5 lines
    QFontMetrics font_metrics(content->currentFont());
    setMinimumHeight(font_metrics.lineSpacing() * 5);

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

void NHMessageWindow::doprev_message()
{
    QFontMetrics font_metrics(content->currentFont());
    QScrollBar *vertical_scrollbar = content->verticalScrollBar();
    vertical_scrollbar->setValue(vertical_scrollbar->value() - font_metrics.lineSpacing());
}
