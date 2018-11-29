extern "C" {
#include "tile2x11.h"
extern short glyph2tile[]; // from tile.c
}

#include <QGroupBox>
#include <QStandardItemModel>

#include "qt5_port.h"
#include "qt5_icons.h"


NHPlayerSelectionDialog::NHPlayerSelectionDialog(QPixmap *tiles, QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    QIcon gender_icons[2] = {
        QPixmap(male_sign_xpm),
        QPixmap(female_sign_xpm),
    };
    QIcon align_icons[3] = {
        QPixmap(lawful_sign_xpm),
        QPixmap(neutral_sign_xpm),
        QPixmap(chaotic_sign_xpm),
    };

    QGroupBox *role_groupbox = new QGroupBox(this);
    role_groupbox->setTitle("Role");
    QHBoxLayout *role_layout = new QHBoxLayout();
    role_layout->setContentsMargins(0, 0, 0, 0);
    role_layout->setSpacing(10);
    role_randomness = new QCheckBox("Random", this);
    role_randomness->setFocusPolicy(Qt::StrongFocus);
    role_randomness->setChecked(true);
    role_layout->addWidget(role_randomness);
    role_selector = new QComboBox(this);
    role_selector->setFocusPolicy(Qt::StrongFocus);
    role_selector->setDisabled(true);
    connect(role_randomness, &QCheckBox::toggled, role_selector, &QComboBox::setDisabled);
    for (int i = 0; roles[i].name.m; i++) {
        QIcon role_icon(tiles[glyph2tile[monnum_to_glyph(roles[i].malenum)]]);
        role_selector->addItem(role_icon, QString(roles[i].name.m));
    }
    role_layout->addWidget(role_selector, 1);
    role_groupbox->setLayout(role_layout);
    main_layout->addWidget(role_groupbox);

    QGroupBox *race_groupbox = new QGroupBox(this);
    race_groupbox->setTitle("Race");
    QHBoxLayout *race_layout = new QHBoxLayout();
    race_layout->setContentsMargins(0, 0, 0, 0);
    race_layout->setSpacing(10);
    race_randomness = new QCheckBox("Random", this);
    race_randomness->setFocusPolicy(Qt::StrongFocus);
    race_randomness->setChecked(true);
    race_layout->addWidget(race_randomness);
    race_selector = new QComboBox(this);
    race_selector->setFocusPolicy(Qt::StrongFocus);
    race_selector->setDisabled(true);
    connect(race_randomness, &QCheckBox::toggled, race_selector, &QComboBox::setDisabled);
    for (int i = 0; races[i].noun; i++) {
        QIcon race_icon(tiles[glyph2tile[monnum_to_glyph(races[i].malenum)]]);
        race_selector->addItem(race_icon, QString(races[i].noun));
    }
    race_layout->addWidget(race_selector, 1);
    race_groupbox->setLayout(race_layout);
    main_layout->addWidget(race_groupbox);

    QGroupBox *gender_groupbox = new QGroupBox(this);
    gender_groupbox->setTitle("Gender");
    QHBoxLayout *gender_layout = new QHBoxLayout();
    gender_layout->setContentsMargins(0, 0, 0, 0);
    gender_layout->setSpacing(10);
    gender_randomness = new QCheckBox("Random", this);
    gender_randomness->setFocusPolicy(Qt::StrongFocus);
    gender_randomness->setChecked(true);
    gender_layout->addWidget(gender_randomness);
    gender_selector = new QComboBox(this);
    gender_selector->setFocusPolicy(Qt::StrongFocus);
    gender_selector->setDisabled(true);
    connect(gender_randomness, &QCheckBox::toggled, gender_selector, &QComboBox::setDisabled);
    for (int i = 0; i < ROLE_GENDERS; i++) {
        gender_selector->addItem(QIcon(gender_icons[i]), QString(genders[i].adj));
    }
    gender_layout->addWidget(gender_selector, 1);
    gender_groupbox->setLayout(gender_layout);
    main_layout->addWidget(gender_groupbox);

    QGroupBox *align_groupbox = new QGroupBox(this);
    align_groupbox->setTitle("Alignment");
    QHBoxLayout *align_layout = new QHBoxLayout();
    align_layout->setContentsMargins(0, 0, 0, 0);
    align_layout->setSpacing(10);
    align_randomness = new QCheckBox("Random", this);
    align_randomness->setFocusPolicy(Qt::StrongFocus);
    align_randomness->setChecked(true);
    align_layout->addWidget(align_randomness);
    align_selector = new QComboBox(this);
    align_selector->setFocusPolicy(Qt::StrongFocus);
    align_selector->setDisabled(true);
    connect(align_randomness, &QCheckBox::toggled, align_selector, &QComboBox::setDisabled);
    for (int i = 0; i < ROLE_ALIGNS; i++) {
        align_selector->addItem(QIcon(align_icons[i]), QString(aligns[i].adj));
    }
    align_layout->addWidget(align_selector, 1);
    align_groupbox->setLayout(align_layout);
    main_layout->addWidget(align_groupbox);

    QHBoxLayout *buttons_layout = new QHBoxLayout();
    buttons_layout->setContentsMargins(0, 0, 0, 0);
    buttons_layout->setSpacing(10);
    QPushButton *reject_button = new QPushButton("Cancel", this);
    QPushButton *validate_button = new QPushButton("Ok", this);
    connect(reject_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(validate_button, &QPushButton::clicked, this, &QDialog::accept);
    buttons_layout->addWidget(reject_button);
    buttons_layout->addWidget(validate_button);
    main_layout->addLayout(buttons_layout);

    validate_button->setDefault(true);
    role_randomness->setFocus();

    connect(role_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NHPlayerSelectionDialog::update_role);
    connect(race_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NHPlayerSelectionDialog::update_race);
}

void NHPlayerSelectionDialog::update_role(int newIndex)
{
    check_accessible_races(roles[newIndex].allow);
    check_accessible_genders(roles[newIndex].allow & races[race_selector->currentIndex()].allow);
    check_accessible_aligns(roles[newIndex].allow & races[race_selector->currentIndex()].allow);
}

void NHPlayerSelectionDialog::update_race(int newIndex)
{
    check_accessible_genders(roles[role_selector->currentIndex()].allow & races[newIndex].allow);
    check_accessible_aligns(roles[role_selector->currentIndex()].allow & races[newIndex].allow);
}


void NHPlayerSelectionDialog::check_accessible_races(short allowed_flags)
{
    QStandardItemModel *race_model = qobject_cast<QStandardItemModel*> (race_selector->model());
    int current_race_index = race_selector->currentIndex();

    bool must_reset = !(allowed_flags & races[current_race_index].selfmask),
         have_reset = false;

    for (int i = 0; races[i].noun; i++) {
        if (allowed_flags & races[i].selfmask) {
            race_model->item(i)->setEnabled(true);
            if (must_reset && !have_reset) {
                race_selector->setCurrentIndex(i);
                have_reset = true;
            }
        } else {
            race_model->item(i)->setEnabled(false);
        }
    }
}

void NHPlayerSelectionDialog::check_accessible_genders(short allowed_flags)
{
    QStandardItemModel *gender_model = qobject_cast<QStandardItemModel*> (gender_selector->model());
    int current_gender_index = gender_selector->currentIndex();

    bool must_reset = !(allowed_flags & genders[current_gender_index].allow),
         have_reset = false;

    for (int i = 0; i < ROLE_GENDERS; i++) {
        if (allowed_flags & genders[i].allow) {
            gender_model->item(i)->setEnabled(true);
            if (must_reset && !have_reset) {
                gender_selector->setCurrentIndex(i);
                have_reset = true;
            }
        } else {
            gender_model->item(i)->setEnabled(false);
        }
    }
}

void NHPlayerSelectionDialog::check_accessible_aligns(short allowed_flags)
{
    QStandardItemModel *align_model = qobject_cast<QStandardItemModel*> (align_selector->model());
    int current_align_index = align_selector->currentIndex();

    bool must_reset = !(allowed_flags & aligns[current_align_index].allow),
         have_reset = false;

    for (int i = 0; i < ROLE_ALIGNS; i++) {
        if (allowed_flags & aligns[i].allow) {
            align_model->item(i)->setEnabled(true);
            if (must_reset && !have_reset) {
                align_selector->setCurrentIndex(i);
                have_reset = true;
            }
        } else {
            align_model->item(i)->setEnabled(false);
        }
    }
}

int NHPlayerSelectionDialog::get_role_index()
{
    if (!role_randomness->isChecked()){
        return role_selector->currentIndex();
    }

    return randrole();
}

int NHPlayerSelectionDialog::get_race_index(int selected_role_idx)
{
    if (!race_randomness->isChecked()) {
        return race_selector->currentIndex();
    }

    return randrace(selected_role_idx);
}

int NHPlayerSelectionDialog::get_gender_index(int selected_role_idx, int selected_race_idx)
{
    if (!gender_randomness->isChecked()) {
        return gender_selector->currentIndex();
    }

    return randgend(selected_role_idx, selected_race_idx);
}

int NHPlayerSelectionDialog::get_align_index(int selected_role_idx, int selected_race_idx)
{
    if (!align_randomness->isChecked()) {
        return align_selector->currentIndex();
    }

    return randalign(selected_role_idx, selected_race_idx);
}
