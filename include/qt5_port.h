#ifndef qt5_port_h
#define qt5_port_h

extern "C" {

#include "hack.h"
#undef min
#undef max
#undef get_ext_cmd
}

#define QT5_MESSAGE_WINDOW 1 << 5
#define QT5_MAP_WINDOW 1 << 6
#define QT5_MENU_WINDOW 1 << 7
#define QT5_TEXT_WINDOW 1 << 8
#define QT5_STATUS_WINDOW 1 << 9

#define QT5_TEXT_ONLY_MENU 1
#define QT5_REGULAR_MENU 2

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
#include <QTextCharFormat>
#include <QLabel>
#include <QVBoxLayout>


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
    QTextCharFormat format;
public:
    NHMessageWindow(QWidget *parent);
    void print_line(const QString line);
    void print_string(const QString str);
    void set_bold();
    void unset_bold();
    void print_yn(const char *ques, const char *choices, int dflt);
    void remove_chars(const int n);
};


class NHMapWindow : public QGraphicsView
{
    Q_OBJECT
private:
    QGraphicsScene *scene;
    QPixmap tiles[MAX_GLYPH];
    QPixmap *petmark;
    int max_x;
    int max_y;
    int tile_size;
    void clear_glyph(int x, int y);
public:
    NHMapWindow(QWidget *parent);
    void clear();
    void ensure_visible(int x, int y);
    void draw_glyph(int x, int y, int glyph);
};


class NHMenuLine : public QWidget {
    Q_OBJECT

private:
    CHAR_P accelerator;
    const ANY_P *identifier;
    BOOLEAN_P selected;

    QLabel *selection_label;
    QLabel *glyph_label;
    QLabel *accelerator_label;
    QLabel *description_label;

// public slots:
//     void toggle_select();

public:
    NHMenuLine(int glyph, const ANY_P *identifier, CHAR_P accelerator, int attr, const char *str, BOOLEAN_P preselected, QWidget *parent);
    int matchesKeyPress(QString typed_str);
    void toggleSelection();
    BOOLEAN_P is_selected();
    MENU_ITEM_P get_menu_item();
};

class NHMenuWindow : public QDialog
{
    Q_OBJECT
    int type;
    QVector<NHMenuLine*> lines;
    QVBoxLayout *content_layout;
    QVBoxLayout *main_layout;
    QHBoxLayout *button_layout;
    QPushButton *accept_button;
    QPushButton *reject_button;
    int cursor_index;

    int how;
    int selection_count;
    MENU_ITEM_P **selection;

public:
    NHMenuWindow(QWidget *parent);
    void done(int r);
    void start();
    void add(int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected);
    void setup_menu_selection(int how, MENU_ITEM_P **selection);
    int get_selection_count();
    void set_menu_prompt(const char *prompt);
    void print_line(int attr, const char *str);
    void keyPressEvent(QKeyEvent *e);
};


class NHTextWindow : public QDockWidget
{
    Q_OBJECT
public:
    NHTextWindow(QWidget *parent);
};

class StatusWidget : public QWidget
{
private:
    int value;
    int timeout;
    QLabel *label;
public:
    StatusWidget(int type, QWidget *parent);
    void update_value(int newval);
    void decrease_timeout();
};

class NHStatusWindow : public QDockWidget
{
    Q_OBJECT
private:
    StatusWidget *str_widget;
    StatusWidget *intl_widget;
    StatusWidget *wis_widget;
    StatusWidget *dex_widget;
    StatusWidget *con_widget;
    StatusWidget *cha_widget;
    QLabel *presentation_label;
    QLabel *gold_label;
    QLabel *hp_label;
    QLabel *power_label;
    QLabel *ac_label;
    QLabel *level_label;
    QLabel *exp_label;
    QLabel *time_label;
    void update_presentation();
    void update_gold();
    void update_hp();
    void update_power();
    void update_ac();
    void update_level();
    void update_exp();
    void update_time();
public:
    NHStatusWindow(QWidget *parent);
    void update_status();
};

class NHMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    NHMainWindow();
    ~NHMainWindow();
    static NHMainWindow *_instance;

    QQueue<int> keybuffer;
    QVector<NHMessageWindow*> message_windows;
    QVector<NHMapWindow*> map_windows;
    QVector<NHMenuWindow*> menu_windows;
    QVector<NHTextWindow*> text_windows;
    QVector<NHStatusWindow*> status_windows;

    void keyPressEvent(QKeyEvent *e);

public:
    static NHMainWindow* instance();

    // window management methods
    winid create_window(int type);
    void clear_window(winid wid);
    void display_window(winid wid, BOOLEAN_P blocking);
    void destroy_window(winid wid);

    // WIN_MAP display methods
    void ensure_visible(int x, int y);
    void draw_glyph(winid wid, int x, int y, int glyph);

    // menu methods
    void init_menu(winid wid);
    void add_menu_entry(winid wid, int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected);
    void flush_menu(winid wid, const char *prompt);
    int get_menu_selection(winid wid, int how, MENU_ITEM_P **selection);

    // player methods
    void select_player();
    void ask_name();

    // general gameplay-related methods
    int getch();
    char yn_function(const char *ques, const char *choices, int dflt);
    int poskey(int *x, int *y, int *mod);
    int get_ext_cmd();
    void display_str(winid wid, int attr, const char *str);
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
