extern "C" {
#include "tile2x11.h"
extern short glyph2tile[]; // from tile.c
}
#include "qt5_port.h"
#include "qt5_icons.h"

#include <QHBoxLayout>
#include <QLabel>

NHMenuLine::NHMenuLine(QPixmap *glyph, const ANY_P *identifier, CHAR_P accelerator, int attr, const char *str, BOOLEAN_P preselected, QWidget *parent) : QWidget(parent)
{
    this->selected = preselected;
    this->accelerator = accelerator;

    if (identifier) {
        this->identifier = *identifier;
    }

    selection_label = new QLabel("", this);
    glyph_label = new QLabel(this);
    if (glyph) {
        glyph_label->setPixmap(*glyph);
    }

    accelerator_label = new QLabel(QString(accelerator), this);
    description_label = new QLabel(str, this);
    if (attr == ATR_BOLD) {
        description_label->setStyleSheet("font-weight: bold;");
    } else if (attr == ATR_ULINE) {
        description_label->setStyleSheet("text-decoration: underline;");
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(selection_label, 0);
    layout->addWidget(accelerator_label, 0);
    layout->addWidget(glyph_label, 0);
    layout->addWidget(description_label, 1);
}

int NHMenuLine::matchesKeyPress(QString typed_str)
{
    return typed_str.size() && identifier.a_void != 0 && accelerator == typed_str.at(0);
}

void NHMenuLine::toggleSelection()
{
    if (identifier.a_void != 0) {
        selected = !selected;
        selection_label->setText(selected ? "+" : "");
    }
}

BOOLEAN_P NHMenuLine::is_selected()
{
    return selected;
}

MENU_ITEM_P NHMenuLine::get_menu_item() {
    MENU_ITEM_P output_item;
    output_item.item = identifier;
    output_item.count = -1; // select all - TODO should be set by the user

    return output_item;
}


NHMenuWindow::NHMenuWindow(QPixmap *tiles, QWidget *parent) : QDialog(parent)
{
    this->tiles = tiles;

    content_layout = new QVBoxLayout();
    content_layout->setSpacing(0);

    button_layout = new QHBoxLayout();
    accept_button = new QPushButton("Ok", this);
    connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
    reject_button = new QPushButton("Cancel", this);
    connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));
    button_layout->addWidget(accept_button);
    button_layout->addWidget(reject_button);

    main_layout = new QVBoxLayout(this);
    main_layout->addLayout(content_layout);
    main_layout->addStretch(1);
    main_layout->addLayout(button_layout);
}

void NHMenuWindow::done(int r) {
    if (type == QT5_REGULAR_MENU) {
        selection_count = 0;
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->is_selected()) {
                selection_count++;
            }
        }

        *selection = new MENU_ITEM_P[get_selection_count()];

        int current_selection = 0;
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->is_selected()) {
                (*selection)[current_selection] = lines.at(i)->get_menu_item();
                current_selection++;
            }
        }
    }

    QLayoutItem *child;
    while ((child = content_layout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    lines.clear();

    QDialog::done(r);
}

void NHMenuWindow::start()
{
    cursor_index = 0;
    type = QT5_REGULAR_MENU;
}

void NHMenuWindow::add(int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected)
{
    QPixmap *tile = nullptr;
    if (glyph != NO_GLYPH) {
        tile = &tiles[glyph2tile[glyph]];
    }

    printf("item identified by %p as %c\n", identifier, accelerator);

    NHMenuLine *new_menu = new NHMenuLine(tile, identifier, accelerator, attr, str, preselected, this);
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
    NHMenuLine *new_menu = new NHMenuLine(0, 0, 0, 0, str, false, this);
    content_layout->addWidget(new_menu);
}

void NHMenuWindow::keyPressEvent(QKeyEvent *e)
{
    QString typed_str = e->text();
    if (typed_str.size()) {
        for (int i = 0; i < lines.size(); i++) {
            if (lines.at(i)->matchesKeyPress(typed_str)) {
                lines.at(i)->toggleSelection();
            }
        }
    } else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) {
        // TODO handle highlight in menu select
    }

    return QDialog::keyPressEvent(e);
}
