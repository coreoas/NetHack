#include <fstream>
#include "qt5_port.h"

extern "C" {
#include "func_tab.h"
#include "tile2x11.h"
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
    // Check that x11tiles file exists
    // If so, initialize the property tiles[]
    if (!std::ifstream("x11tiles").good()) {
        /* file does not exists or is not readable */
        raw_printf("Unable to open x11tiles.\n");
        exit(EXIT_FAILURE);
        return;
    }

    iflags.window_inited = 1;
    QPixmap tilemap("x11tiles", "XPM");
    tile_size = tilemap.width() / 40;

    tiles = new QPixmap[MAX_GLYPH];
    int i, j;
    for (j = 0; j < tilemap.height() / tile_size; j++) {
        for (i = 0; i < TILES_PER_ROW; i++) {
            tiles[i + TILES_PER_ROW * j] = tilemap.copy(i * tile_size,
                                                        j * tile_size,
                                                        tile_size,
                                                        tile_size);
        }
    }

    main_content = new QSplitter(this);
    main_content->setOrientation(Qt::Vertical);
    main_content->setChildrenCollapsible(false);

    central_widget = new QWidget(this);
    central_layout = new QVBoxLayout(central_widget);
    central_layout->setContentsMargins(0, 0, 0, 0);
    central_layout->addWidget(main_content, 1);
    setCentralWidget(central_widget);
}

NHMainWindow::~NHMainWindow() {
    iflags.window_inited = 0;

    delete[] tiles;
}

winid NHMainWindow::create_window(int type)
{
    switch (type){
        case NHW_MESSAGE: {
            NHMessageWindow *new_msg_win = new NHMessageWindow(this);
            message_windows.append(new_msg_win);
            return (winid) (QT5_MESSAGE_WINDOW | (message_windows.size() - 1));
        }

        case NHW_MAP: {
            NHMapWindow *new_map_win = new NHMapWindow(tiles, tile_size, this);
            map_windows.append(new_map_win);
            return (winid) (QT5_MAP_WINDOW | (map_windows.size() - 1));
        }

        case NHW_MENU: {
            NHMenuWindow *new_menu_win = new NHMenuWindow(tiles, this);
            menu_windows.append(new_menu_win);
            return (winid) (QT5_MENU_WINDOW | (menu_windows.size() - 1));
        }

        case NHW_TEXT: {
            NHTextWindow *new_text_win = new NHTextWindow(this);
            text_windows.append(new_text_win);
            return (winid) (QT5_TEXT_WINDOW | (text_windows.size() - 1));
        }
    }
    printf("invalid window type\n");
    return -1;
}

void NHMainWindow::clear_window(winid wid)
{
    if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->clear();
    } else if (QT5_TEXT_WINDOW & wid) {
        text_windows[QT5_TEXT_WINDOW ^ wid]->clear();
    }
}

void NHMainWindow::display_window(winid wid, BOOLEAN_P blocking)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        // TODO handle blocking for message windows, with "--more--"
        if (main_content->widget(0) == nullptr) {
            main_content->insertWidget(0, message_windows[QT5_MESSAGE_WINDOW ^ wid]);
        } else if (main_content->widget(0) != message_windows[QT5_MESSAGE_WINDOW ^ wid]) {
            main_content->replaceWidget(0, message_windows[QT5_MESSAGE_WINDOW ^ wid]);
        }
        main_content->setStretchFactor(0, 0);
    } else if (QT5_MAP_WINDOW & wid) {
        if (main_content->widget(1) == nullptr) {
            main_content->insertWidget(1, map_windows[QT5_MAP_WINDOW ^ wid]);
        } else if (main_content->widget(1) != map_windows[QT5_MAP_WINDOW ^ wid]) {
            main_content->replaceWidget(1, map_windows[QT5_MAP_WINDOW ^ wid]);
        }
        main_content->setStretchFactor(1, 1);
    } else if (QT5_TEXT_WINDOW & wid) {
        if (main_content->widget(1) == nullptr) {
            main_content->insertWidget(1, text_windows[QT5_TEXT_WINDOW ^ wid]);
        } else if (main_content->widget(1) != text_windows[QT5_TEXT_WINDOW ^ wid]) {
            main_content->replaceWidget(1, text_windows[QT5_TEXT_WINDOW ^ wid]);
        }
        main_content->setStretchFactor(1, 1);
        // Whatever the value of blocking, text windows must always wait for dismissal.
        while (true) {
            if (text_windows[QT5_TEXT_WINDOW ^ wid]->is_dismissed())
                break;
            if (!keybuffer.isEmpty()) {
                keybuffer.dequeue();
                break;
            }
            QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
        }

    } else if (QT5_MENU_WINDOW & wid) {
        // special case, the widget is not in the layout but is a QDialog
        menu_windows[QT5_MENU_WINDOW ^ wid]->exec();
    }
    QCoreApplication::processEvents();
}

void NHMainWindow::destroy_window(winid wid)
{
    if (QT5_MESSAGE_WINDOW & wid) {
        // To keep the indexes constant
        // TODO smart index management
        message_windows[QT5_MESSAGE_WINDOW ^ wid]->hide();
        delete message_windows[QT5_MESSAGE_WINDOW ^ wid];
        message_windows[QT5_MESSAGE_WINDOW ^ wid] = nullptr;
    } else if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->hide();
        delete map_windows[QT5_MAP_WINDOW ^ wid];
        map_windows[QT5_MAP_WINDOW ^ wid] = nullptr;
    } else if (QT5_MENU_WINDOW & wid) {
        delete menu_windows[QT5_MENU_WINDOW ^ wid];
        menu_windows[QT5_MENU_WINDOW ^ wid] = nullptr;
    } else if (QT5_TEXT_WINDOW & wid) {
        text_windows[QT5_TEXT_WINDOW ^ wid]->hide();
        delete text_windows[QT5_TEXT_WINDOW ^ wid];
        text_windows[QT5_TEXT_WINDOW ^ wid] = nullptr;
    }

    QCoreApplication::processEvents();
}

void NHMainWindow::init_status()
{
    status_window = new NHStatusWindow(this);
    central_layout->addWidget(status_window, 0);
    QCoreApplication::processEvents();
}

void NHMainWindow::finish_status()
{
    central_layout->removeWidget(status_window);
    delete status_window;
    QCoreApplication::processEvents();
}

void NHMainWindow::enablefield_status(int fldindex, const char *fldname, const char *fieldfmt, BOOLEAN_P enable)
{
    status_window->enable_field(fldindex, fldname, fieldfmt, enable);
}

void NHMainWindow::update_status(int fldindex, genericptr_t ptr, int chg, int percentage, int color, unsigned long *colormasks)
{
    status_window->update_field(fldindex, ptr, chg, percentage, color, colormasks);
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
        map_windows[QT5_MAP_WINDOW ^ wid]->draw_glyph(x, y, glyph);
    }
}

void NHMainWindow::draw_cursor(winid wid, int x, int y)
{
    if (QT5_MAP_WINDOW & wid) {
        map_windows[QT5_MAP_WINDOW ^ wid]->draw_cursor(x, y);
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

int NHMainWindow::doprev_message()
{
    if (WIN_MESSAGE != -1) {
        message_windows[QT5_MESSAGE_WINDOW ^ WIN_MESSAGE]->doprev_message();
    }
    return 0;
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
    } else if (QT5_MENU_WINDOW & wid) {
        menu_windows[QT5_MENU_WINDOW ^ wid]->print_line(attr, str);
    } else if (QT5_TEXT_WINDOW & wid) {
        text_windows[QT5_TEXT_WINDOW ^ wid]->print_line(attr, str);
    }
}


void NHMainWindow::ensure_visible(int x, int y)
{
    if (WIN_MAP != -1)
        map_windows[QT5_MAP_WINDOW ^ WIN_MAP]->ensure_visible(x, y);
}
