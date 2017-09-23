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
