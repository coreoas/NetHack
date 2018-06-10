#include <QScrollBar>
#include <QFontMetrics>
#include "qt5_port.h"

NHMessageWindow::NHMessageWindow(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
    setFocusPolicy(Qt::NoFocus);

    // minimum height is 5 lines
    QFontMetrics font_metrics(currentFont());
    setMinimumHeight(font_metrics.lineSpacing() * 5);
}

void NHMessageWindow::print_line(const QString line)
{
    print_string(line + "\n");
}

void NHMessageWindow::print_string(const QString str)
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    cursor.insertText(str, format);
    ensureCursorVisible();
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
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    int i;
    for (i = 0; i < n; i++) {
        textCursor().deletePreviousChar();
    }
}

void NHMessageWindow::doprev_message()
{
    QFontMetrics font_metrics(currentFont());
    QScrollBar *vertical_scrollbar = verticalScrollBar();
    vertical_scrollbar->setValue(vertical_scrollbar->value() - font_metrics.lineSpacing());
}
