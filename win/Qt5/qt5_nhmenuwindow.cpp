extern "C" {
#include "tile2x11.h"
extern short glyph2tile[]; // from tile.c
}
#include "qt5_port.h"
#include "qt5_icons.h"

#include <QHBoxLayout>
#include <QLabel>
#include <string.h>

NHMenuLine::NHMenuLine(QPixmap *glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P group_accelerator, int attr, const char *str, BOOLEAN_P preselected, QWidget *parent) : QWidget(parent)
{
    this->accelerator = accelerator;
    this->group_accelerator = group_accelerator;

    QHBoxLayout *layout = new QHBoxLayout(this);

    glyph_label = new QLabel(this);
    if (glyph) {
        glyph_label->setPixmap(*glyph);
    }

    if (identifier && identifier->a_long != 0) {
        // The memory of identifier can be freed, it's safer to keep a copy of it.
        this->identifier = new ANY_P;
        *this->identifier = *identifier;
        selection_box = new QCheckBox(this);
        selection_box->setCheckState(preselected ? Qt::Checked : Qt::Unchecked);
        selection_box->setFixedWidth(20);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(selection_box, 0);
    } else {
        this->identifier = nullptr;
        layout->setContentsMargins(20, 0, 0, 0);
    }

    accelerator_label = new QLabel(QString(accelerator), this);
    description_label = new QLabel(str, this);
    if (attr == ATR_BOLD) {
        description_label->setStyleSheet("font-weight: bold;");
    } else if (attr == ATR_ULINE) {
        description_label->setStyleSheet("text-decoration: underline;");
    }

    layout->addWidget(accelerator_label, 0);
    layout->addWidget(glyph_label, 0);
    layout->addWidget(description_label, 1);
}

NHMenuLine::~NHMenuLine()
{
    if (NULL != this->identifier) {
        delete this->identifier;
    }
}

void NHMenuLine::mouseReleaseEvent(QMouseEvent *event)
{
    if (identifier != nullptr && identifier->a_long != 0) {
        selection_box->toggle();
    }

    event->accept();
}

int NHMenuLine::matches_key_press(QString typed_str)
{
    // '@' is a shorthand to select all the items in the list
    return (
        typed_str.size() &&
        identifier != nullptr &&
        identifier->a_long != 0 &&
        (typed_str.at(0) == '@' || typed_str.at(0) == accelerator || typed_str.at(0) == group_accelerator)
    );
}

void NHMenuLine::toggle_selection()
{
    if (identifier != nullptr && identifier->a_long != 0) {
        selection_box->toggle();
    }
}

BOOLEAN_P NHMenuLine::is_selected()
{
    if (identifier != nullptr && identifier->a_long != 0) {
        return selection_box->isChecked();
    }

    return false;
}

MENU_ITEM_P NHMenuLine::get_menu_item() {
    MENU_ITEM_P output_item;
    if (identifier != nullptr) {
        output_item.item = *identifier;
    }
    output_item.count = -1; // select all - TODO should be set by the user

    return output_item;
}


NHMenuWindow::NHMenuWindow(QPixmap *tiles, QWidget *parent) : QDialog(parent)
{
    this->tiles = tiles;
    this->current_accelerator = 'a';

    content_layout = new QVBoxLayout();
    content_layout->setContentsMargins(0, 0, 0, 0);
    content_layout->setSpacing(0);

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->setContentsMargins(10, 10, 10, 10);
    button_layout->setSpacing(10);
    QPushButton *accept_button = new QPushButton("Ok", this);
    connect(accept_button, &QPushButton::clicked, this, &NHMenuWindow::accept);
    QPushButton *reject_button = new QPushButton("Cancel", this);
    connect(reject_button, &QPushButton::clicked, this, &NHMenuWindow::reject);
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    main_widget = new QWidget();
    QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
    main_layout->setSpacing(0);
    main_layout->addLayout(content_layout);
    main_layout->addStretch(1);

    scrollable_area = new QScrollArea(this);
    scrollable_area->setWidgetResizable(true);
    scrollable_area->setWidget(main_widget);
    scrollable_area->setMinimumHeight(100);
    scrollable_area->setFixedWidth(300);
    QVBoxLayout *scrollable_area_layout = new QVBoxLayout(this);
    scrollable_area_layout->setContentsMargins(0, 0, 0, 0);
    scrollable_area_layout->setSpacing(0);
    scrollable_area_layout->addWidget(scrollable_area);
    scrollable_area_layout->addLayout(button_layout);
}

void NHMenuWindow::done(int r) {
    if (type == QT5_REGULAR_MENU) {
        selection_count = 0;
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->is_selected()) {
                selection_count++;
            }
        }

        // As stated in window.doc,
        // freeing this array is the caller's responsibility
        *selection = new MENU_ITEM_P[get_selection_count()];

        int current_selection = 0;
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->is_selected()) {
                (*selection)[current_selection] = lines.at(i)->get_menu_item();
                current_selection++;
            }
        }
    }

    // We also reset the accelerator counter.
    this->current_accelerator = 'a';

    // We clear all the layouts and items they contain.
    QLayoutItem *child;
    while ((child = content_layout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    lines.clear();

    // Finally we reset the size of the scrollable area.
    scrollable_area->setFixedWidth(300);
    scrollable_area->setMinimumHeight(100);

    QDialog::done(r);
}

void NHMenuWindow::start()
{
    cursor_index = 0;
    type = QT5_REGULAR_MENU;
}

int NHMenuWindow::exec()
{
    // opens the dialog, and flushes the heights.
    // This is not the best but is required to manipulate the QScrollArea height
    setModal(true);
    show();

    int content_height = main_widget->geometry().height() + 40;
    if (scrollable_area->height() < content_height) {
        scrollable_area->setMinimumHeight(std::min(content_height, 500));
    }
    int content_width = main_widget->geometry().width() + 40;
    if (scrollable_area->width() < content_width) {
        scrollable_area->setFixedWidth(content_width);
    }

    return QDialog::exec();
}

void NHMenuWindow::add(int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected)
{
    QPixmap *tile = nullptr;
    if (glyph != NO_GLYPH) {
        tile = &tiles[glyph2tile[glyph]];
    }

    CHAR_P chosen_accelerator = accelerator;

    // In case a selectable item has 0 for accelerator
    if (identifier && identifier->a_long != 0 && accelerator == 0 && current_accelerator != 'Z') {
        chosen_accelerator = current_accelerator;
        // current_accelerator is incremented except if it equals 'z', and we restart at 'A'
        current_accelerator = (current_accelerator == 'z') ? 'A' : (current_accelerator + 1);
    }

    NHMenuLine *new_menu = new NHMenuLine(tile, identifier, chosen_accelerator, groupacc, attr, str, preselected, this);
    content_layout->addWidget(new_menu);
    lines.append(new_menu);
}

void NHMenuWindow::set_menu_prompt(const char *prompt)
{
    content_layout->insertWidget(0, new QLabel(prompt, this));
}

int NHMenuWindow::get_selection_count()
{
    return selection_count;
}

void NHMenuWindow::setup_menu_selection(int how, MENU_ITEM_P **selection) {
    this->how = how;
    this->selection = selection;
}

void NHMenuWindow::print_line(int attr, const char *str)
{
    type = QT5_TEXT_ONLY_MENU;
    NHMenuLine *new_menu = new NHMenuLine(0, 0, 0, 0, 0, str, false, this);
    content_layout->addWidget(new_menu);
}

void NHMenuWindow::keyPressEvent(QKeyEvent *e)
{
    QString typed_str = e->text();
    if (typed_str.size()) {
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->matches_key_press(typed_str)) {
                lines.at(i)->toggle_selection();
            }
        }
    } else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
        // TODO handle highlight in menu select
    }

    return QDialog::keyPressEvent(e);
}
