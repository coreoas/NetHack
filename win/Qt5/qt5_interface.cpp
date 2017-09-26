extern "C" {
    #include "hack.h"
}

#include "qt5_port.h"


void qt5_init_nhwindows(int* argc, char** argv)
{
    NHApplication::instantiate(*argc, argv);
    NHMainWindow* mw = NHMainWindow::instance();
    mw->resize(320, 240);
    mw->show();
    return;
}

void qt5_player_selection()
{
    NHMainWindow::instance()->select_player();
    return;
}

void qt5_askname()
{
    NHMainWindow::instance()->ask_name();
    return;
}

void qt5_get_nh_event()
{
    // noop
    return;
}

void qt5_exit_nhwindows(const char *str)
{
    return;
}

void qt5_suspend_nhwindows(const char *str)
{
    return;
}

void qt5_resume_nhwindows()
{
    return;
}

winid qt5_create_nhwindow(int type)
{
    return NHMainWindow::instance()->create_window(type);
}

void qt5_clear_nhwindow(winid wid)
{
    return;
}

void qt5_display_nhwindow(winid wid, BOOLEAN_P blocking)
{
    NHMainWindow::instance()->display_window(wid, blocking);
    return;
}

void qt5_destroy_nhwindow(winid wid)
{
    NHMainWindow::instance()->destroy_window(wid);
    return;
}

void qt5_curs(winid wid, int x, int y)
{
    return;
}

void qt5_putstr(winid wid, int attr, const char *str)
{
    return;
}

void qt5_putmixed(winid wid, int attr, const char *str)
{
    return;
}

void qt5_display_file(const char *str, BOOLEAN_P complain)
{
    return;
}

void qt5_start_menu(winid wid)
{
    return;
}

void qt5_add_menu(winid wid, int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected)
{
    return;
}

void qt5_end_menu(winid wid, const char *prompt)
{
    return;
}

int qt5_select_menu(winid wid, int how, MENU_ITEM_P **selected)
{
    return 0;
}

char qt5_message_menu(CHAR_P let, int how, const char *mesg)
{
    return 0;
}

void qt5_update_inventory()
{
    return;
}

void qt5_mark_synch()
{
    return;
}

void qt5_wait_synch()
{
    return;
}

void qt5_cliparound(int x, int y)
{
    return;
}

void qt5_update_positionbar(char *features)
{
    return;
}

void qt5_print_glyph(winid wid, XCHAR_P x, XCHAR_P y, int glyph, int bkglyph)
{
    return NHMainWindow::instance()->draw_glyph(wid, (int) x, (int) y, glyph);
}

void qt5_raw_print(const char *str)
{
    printf("%s\n", str);
    return;
}

void qt5_raw_print_bold(const char *str)
{
    printf("%s\n", str);
    return;
}

int qt5_nhgetch()
{
    return NHMainWindow::instance()->getch();
}

int qt5_nh_poskey(int *x, int *y, int *mod)
{
    return 0;
}

void qt5_nhbell()
{
    return;
}

int qt5_doprev_message()
{
    return 0;
}

char qt5_yn_function(const char *ques, const char *choices, CHAR_P dflt)
{
    return 0;
}

void qt5_getlin(const char *ques, char *input)
{
    return;
}

int qt5_get_ext_cmd()
{
    return 0;
}

void qt5_number_pad(int)
{
    return;
}

void qt5_delay_output()
{
    return;
}

void qt5_start_screen()
{
    return;
}

void qt5_end_screen()
{
    return;
}

void qt5_outrip(winid wid, int i, time_t t)
{
    return;
}

void qt5_preference_update(const char *prefs)
{
    return;
}

char* qt5_getmsghistory(BOOLEAN_P init)
{
    return (char *) 0;
}

void qt5_putmsghistory(const char *str, BOOLEAN_P enable)
{
    if (enable) {
        printf("%s", str);
    }
    return;
}

void qt5_status_init()
{
    return;
}

void qt5_status_finish()
{
    return;
}

void qt5_status_enablefield(int fldindex, const char *fldname, const char *fieldfmt, BOOLEAN_P enable)
{
    return;
}

void qt5_status_update(int fldindex, genericptr_t ptr, int chg, int percentage, int color, unsigned long *colormasks)
{
    return;
}

boolean qt5_can_suspend()
{
    return true;
}

extern "C" struct window_procs Qt5_procs;

struct window_procs Qt5_procs = {
    "Qt5",
    WC_COLOR|WC_HILITE_PET|
    WC_ASCII_MAP|WC_TILED_MAP|
    WC_FONT_MAP|WC_TILE_FILE|WC_TILE_WIDTH|WC_TILE_HEIGHT|
    WC_PLAYER_SELECTION|WC_SPLASH_SCREEN,
    0L,
    qt5_init_nhwindows,
    qt5_player_selection,
    qt5_askname,
    qt5_get_nh_event,
    qt5_exit_nhwindows,
    qt5_suspend_nhwindows,
    qt5_resume_nhwindows,
    qt5_create_nhwindow,
    qt5_clear_nhwindow,
    qt5_display_nhwindow,
    qt5_destroy_nhwindow,
    qt5_curs,
    qt5_putstr,
    qt5_putmixed,
    qt5_display_file,
    qt5_start_menu,
    qt5_add_menu,
    qt5_end_menu,
    qt5_select_menu,
    qt5_message_menu,
    qt5_update_inventory,
    qt5_mark_synch,
    qt5_wait_synch,
#ifdef CLIPPING
    qt5_cliparound,
#endif
#ifdef POSITIONBAR
    qt5_update_positionbar,
#endif
    qt5_print_glyph,
    qt5_raw_print,
    qt5_raw_print_bold,
    qt5_nhgetch,
    qt5_nh_poskey,
    qt5_nhbell,
    qt5_doprev_message,
    qt5_yn_function,
    qt5_getlin,
    qt5_get_ext_cmd,
    qt5_number_pad,
    qt5_delay_output,
#ifdef CHANGE_COLOR
    noop,
#ifdef MAC
    noop,
    noop,
#endif
    noop,
#endif
    qt5_start_screen,
    qt5_end_screen,
    qt5_outrip,
    qt5_preference_update,
    qt5_getmsghistory,
    qt5_putmsghistory,
    qt5_status_init,
    qt5_status_finish,
    qt5_status_enablefield,
    qt5_status_update,
    qt5_can_suspend
};
