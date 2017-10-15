#include "qt5_port.h"

extern "C" {
#include "func_tab.h"
}


NHMainWindow* NHMainWindow::_instance = nullptr;

NHMainWindow* NHMainWindow::instance()
{
    if (!_instance) {
        _instance = new NHMainWindow();
    }
    return _instance;
}

NHMainWindow::NHMainWindow() : QMainWindow() {
    iflags.window_inited = 1;
}

NHMainWindow::~NHMainWindow() {
    iflags.window_inited = 0;
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

        case NHW_STATUS: {
            NHStatusWindow *new_status_win = new NHStatusWindow(this);
            addDockWidget(Qt::BottomDockWidgetArea, new_status_win);
            new_status_win->hide();
            status_windows.append(new_status_win);
            return (winid) (QT5_STATUS_WINDOW | (status_windows.size() - 1));
        }
    }
    printf("invalid window type\n");
    return -1;
}

void NHMainWindow::clear_window(winid wid)
{
    if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->clear();
    }
}

void NHMainWindow::display_window(winid wid, BOOLEAN_P blocking)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        // TODO handle blocking for message windows, with "--more--"
        message_windows[QT5_MESSAGE_WINDOW ^ wid]->show();
    } else if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->show();
    } else if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->exec();
    } else if (QT5_TEXT_WINDOW & wid) {
        text_windows[QT5_TEXT_WINDOW ^ wid]->show();
    } else if (QT5_STATUS_WINDOW & wid) {
        status_windows[QT5_STATUS_WINDOW ^ wid]->show();
    }
    QCoreApplication::processEvents();
}

void NHMainWindow::destroy_window(winid wid)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        removeDockWidget(message_windows[QT5_MESSAGE_WINDOW ^ wid]);
        // To keep the indexes constant
        // TODO smart index management
        delete message_windows[QT5_MESSAGE_WINDOW ^ wid];
        message_windows[QT5_MESSAGE_WINDOW ^ wid] = nullptr;
    } else if (QT5_MAP_WINDOW & wid) {
        takeCentralWidget();
        delete map_windows[QT5_MAP_WINDOW ^ wid];
        map_windows[QT5_MAP_WINDOW ^ wid] = nullptr;
    } else if (QT5_MENU_WINDOW & wid) {
        delete menu_windows[QT5_MENU_WINDOW ^ wid];
        menu_windows[QT5_MENU_WINDOW ^ wid] = nullptr;
    } else if (QT5_TEXT_WINDOW & wid) {
        removeDockWidget(text_windows[QT5_TEXT_WINDOW ^ wid]);
        delete text_windows[QT5_TEXT_WINDOW ^ wid];
        text_windows[QT5_TEXT_WINDOW ^ wid] = nullptr;
    } else if (QT5_STATUS_WINDOW & wid) {
        removeDockWidget(status_windows[QT5_STATUS_WINDOW ^ wid]);
        delete status_windows[QT5_STATUS_WINDOW ^ wid];
        status_windows[QT5_STATUS_WINDOW ^ wid] = nullptr;
    }

    QCoreApplication::processEvents();
}

void NHMainWindow::init_menu(winid wid)
{
    if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->start();
    } else {
        printf("That's not gonna work :/\n");
    }
}

void NHMainWindow::add_menu_entry(winid wid, int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected)
{
    if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->add(glyph, identifier, accelerator, groupacc, attr, str, preselected);
    } else {
        printf("That's not gonna work :/\n");
    }
}

void NHMainWindow::flush_menu(winid wid, const char *prompt)
{
    if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->set_menu_prompt(prompt);
    } else {
        printf("That's not gonna work :/\n");
    }
}

int NHMainWindow::get_menu_selection(winid wid, int how, MENU_ITEM_P **selection)
{
    menu_windows[QT5_MENU_WINDOW ^ wid]->setup_menu_selection(how, selection);
    if (menu_windows[QT5_MENU_WINDOW ^ wid]->exec()) {
        return menu_windows[QT5_MENU_WINDOW ^ wid]->get_selection_count();
    };

    return -1;
}

void NHMainWindow::select_player()
{
    ask_name();
}

void NHMainWindow::ask_name()
{
    std::strcpy(plname, "wizard");
}


void NHMainWindow::keyPressEvent(QKeyEvent *e)
{
    QString typed_str = e->text();
    int key = e->key();
    if (typed_str.length() == 1) {
        return keybuffer.enqueue(typed_str.at(0).unicode());
    } else if (typed_str.length() == 0 && key < 128) {
        Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
        if (modifiers & Qt::ControlModifier) {
            return keybuffer.enqueue(0x1f & key);
        }
         if (modifiers & Qt::MetaModifier) {
            if (key >= 0x41 && key <= 0x5A && !(modifiers & Qt::ShiftModifier)) {
                // manage meta with or without shift for [A-Z]
                key ^= 0x20;
            }
#ifndef NHSTDC
            return keybuffer.enqueue(0x80 | key);
#else
            return keybuffer.enqueue(key - 128);
#endif
         }
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
    if (WIN_MESSAGE != -1) {
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
            if (response == 0x20 || response == 0x0D || response == 0x7F) {
                response = dflt;
                break;
            }
            msg_win->print_line(QString(response));
        }
        msg_win->print_line(QString(response));
        return response;
    } else {
        return dflt;
    }
}


int NHMainWindow::poskey(int *x, int *y, int *mod)
{
    // TODO implement mouse events
    return getch();
}

int NHMainWindow::get_ext_cmd()
{
    if (QT5_MESSAGE_WINDOW & WIN_MESSAGE) {
        QVector<QString> cmds;
        struct ext_func_tab *efp;
        for (efp = extcmdlist; efp->ef_txt; efp++) {
            if ((!wizard && (efp->flags & WIZMODECMD)) || !(efp->flags & AUTOCOMPLETE)) {
                // to preserve indices
                cmds.append("");
                continue;
            }
            cmds.append(efp->ef_txt);
        }

        NHMessageWindow *msg_win = message_windows[QT5_MESSAGE_WINDOW ^ WIN_MESSAGE];
        msg_win->set_bold();
        msg_win->print_string("#");
        QString typed_chars, autocomplete_chars;
        QVector<int> possible_cmds;
        QString selected_cmd;

        while(true) {
            int new_char = getch();
            if (new_char == 0x1B) {
                msg_win->print_string("\n");
                msg_win->unset_bold();
                return -1;
            }
            if (new_char == 0x0D) {
                msg_win->print_string("\n");
                msg_win->unset_bold();
                int i;
                for (i = 0; i < cmds.size(); i++) {
                    if (cmds.at(i) == typed_chars + autocomplete_chars) {
                        return i;
                    }
                }
                return -1;
            }
            if (new_char != 0x7F) {
                msg_win->print_string(QString(new_char));
                QTextStream(&typed_chars) << autocomplete_chars << (char) new_char;

                autocomplete_chars.clear();

                // autocomplete if we match only one command
                int i;
                possible_cmds.clear();
                for (i = 0; i < cmds.size(); i++) {
                    if (cmds.at(i).startsWith(typed_chars)) {
                        possible_cmds.append(i);
                    }
                }
                if (possible_cmds.size() == 1) {
                    selected_cmd = cmds.at(possible_cmds.at(0));
                    autocomplete_chars = selected_cmd.right(selected_cmd.length() - typed_chars.length());
                    msg_win->print_string(autocomplete_chars);
                }
            } else {
                if (autocomplete_chars.length()) {
                    msg_win->remove_chars(autocomplete_chars.length());
                    autocomplete_chars.clear();
                } else if (typed_chars.length()) {
                    typed_chars.chop(1);
                    msg_win->remove_chars(1);
                }
            }
        }
    }
    return -1;
}

void NHMainWindow::display_str(winid wid, int attr, const char *str)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        message_windows[QT5_MESSAGE_WINDOW ^ wid]->print_line(str);
    } else if (QT5_STATUS_WINDOW & wid) {
        status_windows[QT5_STATUS_WINDOW ^ wid]->update_status();
    } else if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->print_line(attr, str);
    }
    printf("display %s on %d\n", str, wid);
}


void NHMainWindow::ensure_visible(int x, int y)
{
    if (WIN_MAP != -1)
        map_windows[QT5_MAP_WINDOW ^ WIN_MAP]->ensure_visible(x, y);
}
