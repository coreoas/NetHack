#include "qt5_port.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFontDatabase>

NHTextWindow::NHTextWindow(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(QT5_MIN_CENTRAL_WIDGET_WIDTH);
    setMinimumHeight(QT5_MIN_CENTRAL_WIDGET_HEIGHT);

    QVBoxLayout *layout = new QVBoxLayout(this);
    content = new QTextEdit(this);
    dismiss_button = new QPushButton("Dismiss", this);
    connect(dismiss_button, &QPushButton::clicked, this, &NHTextWindow::dismiss);

    content->setReadOnly(true);
    content->setFocusPolicy(Qt::NoFocus);

    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(12);
    format.setFont(font);

    layout->addWidget(content, 1);
    layout->addWidget(dismiss_button, 0);

    _dismiss = false;
}

void NHTextWindow::print_line(int attr, const char *str)
{
    // just in case, resets dismissal state if content changes
    _dismiss = false;

    QTextCursor cursor = content->textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    content->setTextCursor(cursor);
    cursor.insertText(QString(str) + "\n", format);
    content->ensureCursorVisible();
}

void NHTextWindow::clear()
{
    _dismiss = false;
    content->clear();
}

void NHTextWindow::dismiss()
{
    _dismiss = true;
}

boolean NHTextWindow::is_dismissed()
{
    return _dismiss;
}
