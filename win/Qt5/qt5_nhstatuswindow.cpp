// #include "qt_xpms.h"
#include "qt5_port.h"
#include <QHBoxLayout>

QMap<int,int> NHStatusWindow::label_order({
    // 1st row
    {BL_TITLE, 0}, {BL_STR, 1}, {BL_DX, 2}, {BL_CO, 3}, {BL_IN, 4},
    {BL_WI, 5}, {BL_CH, 6}, {BL_ALIGN, 7}, {BL_SCORE, 8},

    // 2nd row
    {BL_LEVELDESC, 9}, {BL_GOLD, 10}, {BL_HP, 11}, {BL_HPMAX, 12},
    {BL_ENE, 13}, {BL_ENEMAX, 14}, {BL_AC, 15}, {BL_XP, 16}, {BL_EXP, 17},
    {BL_HD, 18}, {BL_TIME, 19}, {BL_HUNGER, 20}, {BL_CAP, 21}, {BL_CONDITION, 22}
});

NHStatusWindow::NHStatusWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    first_row = new QHBoxLayout();
    second_row = new QHBoxLayout();
    first_row->setAlignment(Qt::AlignLeft);
    second_row->setAlignment(Qt::AlignLeft);

    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->addLayout(first_row);
    main_layout->addLayout(second_row);

    int i;
    for (i = 0; i < label_order.size(); i++) {
        QLabel *new_label = new QLabel("", this);
        status_labels.append(new_label);
        new_label->hide();
        new_label->setStyleSheet("font-family: monospace; font-size: 12px;");
        if (i <= 8) {
            first_row->addWidget(new_label);
        } else {
            second_row->addWidget(new_label);
        }
    }
}

void NHStatusWindow::enable_field(int fldindex, const char *fldname, const char *fieldfmt, BOOLEAN_P enable)
{
    if (enable) {
        status_labels.at(label_order[fldindex])->show();
    } else {
        status_labels.at(label_order[fldindex])->hide();
    }
}

void NHStatusWindow::update_field(int fldindex, genericptr_t ptr, int chg, int percentage, int color, unsigned long *colormasks)
{
    if (fldindex == BL_FLUSH) {
        return;
    }

    QString content;
    if (fldindex == BL_CONDITION) {
        // TODO manage separators between conditions
        long condition = *((long *) ptr);
        content.append(condition & BL_MASK_STONE ? "Stone" : "");
        content.append(condition & BL_MASK_SLIME ? "Slime" : "");
        content.append(condition & BL_MASK_STRNGL ? "Strngl" : "");
        content.append(condition & BL_MASK_FOODPOIS ? "FoodPois" : "");
        content.append(condition & BL_MASK_TERMILL ? "TermIll" : "");
        content.append(condition & BL_MASK_BLIND ? "Blind" : "");
        content.append(condition & BL_MASK_DEAF ? "Deaf" : "");
        content.append(condition & BL_MASK_STUN ? "Stun" : "");
        content.append(condition & BL_MASK_CONF ? "Conf" : "");
        content.append(condition & BL_MASK_HALLU ? "Hallu" : "");
        content.append(condition & BL_MASK_LEV ? "Lev" : "");
        content.append(condition & BL_MASK_FLY ? "Fly" : "");
        content.append(condition & BL_MASK_RIDE ? "Ride" : "");
    } else {
        switch (fldindex) {
        case BL_TITLE:
        case BL_HUNGER:
        case BL_LEVELDESC:
        case BL_CAP:
            content.sprintf("%s", (char *) ptr);
            break;
        case BL_STR:
            content.sprintf("Str: %s", (char *) ptr);
            break;
        case BL_DX:
            content.sprintf("Dx: %s", (char *) ptr);
            break;
        case BL_CO:
            content.sprintf("Co: %s", (char *) ptr);
            break;
        case BL_IN:
            content.sprintf("In: %s", (char *) ptr);
            break;
        case BL_WI:
            content.sprintf("Wi: %s", (char *) ptr);
            break;
        case BL_CH:
            content.sprintf("Ch: %s", (char *) ptr);
            break;
        case BL_ALIGN:
            content.sprintf("Align: %s", (char *) ptr);
            break;
        case BL_SCORE:
            content.sprintf("Score: %s", (char *) ptr);
            break;
        case BL_GOLD:
            // For BL_GOLD, the value is formated with special char like
            // \GXXXXNNNN that is not handled here (see how genl_putmixed works)
            content.sprintf("$: %s", ((char *) ptr) + 11);
            break;
        case BL_ENE:
            content.sprintf("Pw: %s", (char *) ptr);
            break;
        case BL_ENEMAX:
            content.sprintf("(%s)", (char *) ptr);
            break;
        case BL_XP:
            content.sprintf("Lvl: %s", (char *) ptr);
            break;
        case BL_AC:
            content.sprintf("AC: %s", (char *) ptr);
            break;
        case BL_HD:
            content.sprintf("HD: %s", (char *) ptr);
            break;
        case BL_TIME:
            content.sprintf("Time: %s", (char *) ptr);
            break;
        case BL_HP:
            content.sprintf("HP: %s", (char *) ptr);
            break;
        case BL_HPMAX:
            content.sprintf("(%s)", (char *) ptr);
            break;
        case BL_EXP:
            content.sprintf("Xp: %s", (char *) ptr);
            break;
        }
    }
    status_labels.at(label_order[fldindex])->setText(content);
}
