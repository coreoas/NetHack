#ifndef qt5_win_h
#define qt5_win_h

extern "C" {

#include "hack.h"
#undef min
#undef max

}

#define QT5_MESSAGE_WINDOW 2 << 5
#define QT5_MAP_WINDOW 2 << 6
#define QT5_MENU_WINDOW 2 << 7
#define QT5_TEXT_WINDOW 2 << 8

#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <QQueue>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QVector>
#include <QDockWidget>
#include <QTextEdit>
#include <QTextStream>


static const char *pet_mark_xpm[] = {
/* width height ncolors chars_per_pixel */
"5 5 2 1",
/* colors */
". c None",
"  c #FF0000",
/* pixels */
". . .",
"     ",
"     ",
".   .",
".. ..",
};

class NHApplication : public QApplication
{
    Q_OBJECT
private:
    static NHApplication *_instance;
    NHApplication(int &argc, char **argv);

public:
    static void instantiate(int &argc, char **argv);
    static NHApplication *get_instance();
};


class NHMessageWindow : public QDockWidget
{
    Q_OBJECT
private:
    QTextEdit *content;
public:
    NHMessageWindow(QWidget *parent);
    void print_line(const QString line);
    void print_string(const QString str);
    void print_yn(const char *ques, const char *choices, int dflt);
};


class NHMapWindow : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene *scene;
    QPixmap tiles[MAX_GLYPH];
    QPixmap *pet_mark;
    int tile_size;
    void clear_glyph(int x, int y);
public:
    NHMapWindow(QWidget *parent);
    void draw_glyph(int x, int y, int glyph);
};


class NHMenuWindow : public QDockWidget
{
    Q_OBJECT
public:
    NHMenuWindow(QWidget *parent);
};


class NHTextWindow : public QDockWidget
{
    Q_OBJECT
public:
    NHTextWindow(QWidget *parent);
};

class NHMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    static NHMainWindow *_instance;

    QQueue<int> keybuffer;
    QVector<NHMessageWindow*> message_windows;
    QVector<NHMapWindow*> map_windows;
    QVector<NHMenuWindow*> menu_windows;
    QVector<NHTextWindow*> text_windows;

    void keyPressEvent(QKeyEvent *e);

public:
    static NHMainWindow* instance();

    winid create_window(int type);
    void display_window(winid wid, BOOLEAN_P blocking);
    void destroy_window(winid wid);
    void draw_glyph(winid wid, int x, int y, int glyph);
    void select_player();
    void ask_name();
    int getch();
    char yn_function(const char *ques, const char *choices, int dflt);
    int poskey(int *x, int *y, int *mod);
};


void qt5_init_nhwindows(int* argc, char** argv);
void qt5_player_selection();
void qt5_askname();
void qt5_get_nh_event();
void qt5_exit_nhwindows(const char *str);
void qt5_suspend_nhwindows(const char *str);
void qt5_resume_nhwindows();
winid qt5_create_nhwindow(int type);
void qt5_clear_nhwindow(winid wid);
void qt5_display_nhwindow(winid wid, BOOLEAN_P blocking);
void qt5_destroy_nhwindow(winid wid);
void qt5_curs(winid wid, int x, int y);
void qt5_putstr(winid wid, int attr, const char *str);
void qt5_putmixed(winid wid, int attr, const char *str);
void qt5_display_file(const char *str, BOOLEAN_P complain);
void qt5_start_menu(winid wid);
void qt5_add_menu(winid wid, int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc,
                             int attr, const char *str, BOOLEAN_P preselected);
void qt5_end_menu(winid wid, const char *prompt);
int qt5_select_menu(winid wid, int how, MENU_ITEM_P **selected);
char qt5_message_menu(CHAR_P let, int how, const char *mesg);
void qt5_update_inventory();
void qt5_mark_synch();
void qt5_wait_synch();
void qt5_cliparound(int x, int y);
void qt5_update_positionbar(char *features);
void qt5_print_glyph(winid wid, XCHAR_P x, XCHAR_P y, int glyph, int bkglyph);
void qt5_raw_print(const char *str);
void qt5_raw_print_bold(const char *str);
int qt5_nhgetch();
int qt5_nh_poskey(int *x, int *y, int *mod);
void qt5_nhbell();
int qt5_doprev_message();
char qt5_yn_function(const char *ques, const char *choices, CHAR_P dflt);
void qt5_getlin(const char *ques, char *input);
int qt5_get_ext_cmd();
void qt5_number_pad(int);
void qt5_delay_output();

void qt5_start_screen();
void qt5_end_screen();

void qt5_outrip(winid wid, int i, time_t t);
void qt5_preference_update(const char *prefs);
char* qt5_getmsghistory(BOOLEAN_P init);
void qt5_putmsghistory(const char *str, BOOLEAN_P enable);
void qt5_status_init();
void qt5_status_finish();
void qt5_status_enablefield(int fldindex, const char *fldname, const char *fieldfmt, BOOLEAN_P enable);
void qt5_status_update(int fldindex, genericptr_t ptr, int chg, int percentage, int color, unsigned long *colormasks);
boolean qt5_can_suspend();

#endif
