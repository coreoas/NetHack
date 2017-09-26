#include "qt5_port.h"

NHMainWindow* NHMainWindow::_instance = nullptr;

NHMainWindow* NHMainWindow::instance()
{
    if (!_instance) {
        _instance = new NHMainWindow();
    }
    return _instance;
}

winid NHMainWindow::create_window(int type)
{
    switch (type){
        case NHW_MESSAGE: {
            NHMessageWindow *new_msg_win = new NHMessageWindow(this);
            addDockWidget(Qt::TopDockWidgetArea, new_msg_win);
            new_msg_win->hide();
            message_windows.append(new_msg_win);
            return (winid) (QT5_MESSAGE_WINDOW | (message_windows.size() - 1));
        }

        case NHW_MAP: {
            NHMapWindow *new_map_win = new NHMapWindow(this);
            setCentralWidget(new_map_win);
            new_map_win->hide();
            map_windows.append(new_map_win);
            return (winid) (QT5_MAP_WINDOW | (map_windows.size() - 1));
        }

        case NHW_MENU: {
            NHMenuWindow *new_menu_win = new NHMenuWindow(this);
            addDockWidget(Qt::LeftDockWidgetArea, new_menu_win);
            new_menu_win->hide();
            menu_windows.append(new_menu_win);
            return (winid) (QT5_MENU_WINDOW | (menu_windows.size() - 1));
        }

        case NHW_TEXT: {
            // TODO this is not the intended behaviour
            NHTextWindow *new_text_win = new NHTextWindow(this);
            addDockWidget(Qt::TopDockWidgetArea, new_text_win);
            new_text_win->hide();
            text_windows.append(new_text_win);
            return (winid) (QT5_TEXT_WINDOW | (text_windows.size() - 1));
        }
    }
}

void NHMainWindow::display_window(winid wid, BOOLEAN_P blocking)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        message_windows[QT5_MESSAGE_WINDOW ^ wid]->show();
    } else if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->show();
    } else if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->show();
    } else if (QT5_TEXT_WINDOW & wid) {
        text_windows[QT5_TEXT_WINDOW ^ wid]->show();
    }
    QCoreApplication::processEvents();
}

void NHMainWindow::destroy_window(winid wid)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        removeDockWidget(message_windows[QT5_MAP_WINDOW ^ wid]);
        // To keep the indexes constant
        // TODO smart index management
        delete message_windows[QT5_MESSAGE_WINDOW ^ wid];
        message_windows[QT5_MESSAGE_WINDOW ^ wid] = nullptr;
    } else if (QT5_MAP_WINDOW & wid) {
        takeCentralWidget();
        delete map_windows[QT5_MAP_WINDOW ^ wid];
        map_windows[QT5_MAP_WINDOW ^ wid] = nullptr;
    } else if (QT5_MENU_WINDOW & wid) {
        removeDockWidget(menu_windows[QT5_MENU_WINDOW ^ wid]);
        delete menu_windows[QT5_MENU_WINDOW ^ wid];
        menu_windows[QT5_MENU_WINDOW ^ wid] = nullptr;
    } else if (QT5_TEXT_WINDOW & wid) {
        removeDockWidget(text_windows[QT5_TEXT_WINDOW ^ wid]);
        delete text_windows[QT5_TEXT_WINDOW ^ wid];
        text_windows[QT5_TEXT_WINDOW ^ wid] = nullptr;
    }

    QCoreApplication::processEvents();
}

void NHMainWindow::select_player()
{
    ask_name();
}

void NHMainWindow::ask_name()
{
    std::strcpy(plname, "corentin");
}


void NHMainWindow::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    if (key < 128) {
        Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
        if (!(modifiers & Qt::ShiftModifier)) {
            key = tolower(key);
        }

        if (modifiers == Qt::ShiftModifier) {
            // If the only modifier is Shift,
            // we have to capture the text because it depends on the user's keyboard layout
            return keybuffer.enqueue(key);
        }
        if (modifiers & Qt::ControlModifier) {
            return keybuffer.enqueue(0x1f & key);
        }
        if (modifiers & Qt::MetaModifier) {
#ifndef NHSTDC
            return keybuffer.enqueue(0x80 | key);
#else
            return keybuffer.enqueue(key - 128);
#endif
        }
        return keybuffer.enqueue(key);
    } else if (key == Qt::Key_Escape) {
        return keybuffer.enqueue(0x1B);
    } else if (key == Qt::Key_Return) {
        return keybuffer.enqueue(0x0A);
    } else if (key == Qt::Key_Backspace) {
        return keybuffer.enqueue(0x08);
    }

    return QWidget::keyPressEvent(e);
}

int NHMainWindow::getch()
{
    while (keybuffer.isEmpty()) {
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
    }
    return keybuffer.dequeue();
}

void NHMainWindow::draw_glyph(winid wid, int x, int y, int glyph)
{
    if (QT5_MAP_WINDOW & wid) {
        return map_windows[QT5_MAP_WINDOW ^ wid]->draw_glyph(x, y, glyph);
    } else {
        printf("That's not gonna work :/\n");
    }
}

char NHMainWindow::yn_function(const char *ques, const char *choices, int dflt)
{
    // TODO implement counts
    if (QT5_MESSAGE_WINDOW & WIN_MESSAGE) {
        int response;
        NHMessageWindow *msg_win = message_windows[QT5_MESSAGE_WINDOW ^ WIN_MESSAGE];
        while (true) {
            msg_win->print_yn(ques, choices, dflt);
            response = getch();
            if (choices == nullptr) {
                break;
            }

            if (strchr(choices, tolower(response)) != nullptr) {
                response = tolower(response);
                break;
            }

            if (response == 0x1B) {
                if (strchr(choices, 'q')) {
                    response = 'q';
                } else if (strchr(choices, 'n')) {
                    response = 'n';
                } else {
                    response = dflt;
                }
                break;
            }
            if (response == 0x20 || response == 0x0A || response == 0x08) {
                response = dflt;
                break;
            }
            msg_win->print_line(QString(response));
        }
        msg_win->print_line(QString(response));
        return response;
    } else {
        printf("Standard message window does not exist...\n");
        return dflt;
    }
}


int NHMainWindow::poskey(int *x, int *y, int *mod)
{
    // TODO implement mouse events
    return getch();
}
