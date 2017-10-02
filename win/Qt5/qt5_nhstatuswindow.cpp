#include "qt_xpms.h"
#include "qt5_port.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QRegExp>


QString capitalize(QString input) {
    QStringList parts = input.split(' ', QString::SkipEmptyParts);
    int i;
    for (i = 0; i < parts.size(); i++)
        parts[i].replace(0, 1, parts[i][0].toUpper());

    return parts.join(" ");
}

StatusWidget::StatusWidget(int type, QWidget *parent) : QWidget(parent)
{
    QPixmap icon;
    QLabel *name_label = new QLabel();
    label = new QLabel();
    switch(type) {
        case A_STR:
            name_label->setText("STR: ");
            icon = QPixmap(str_xpm);
            break;
        case A_INT:
            name_label->setText("INT: ");
            icon = QPixmap(int_xpm);
            break;
        case A_WIS:
            name_label->setText("WIS: ");
            icon = QPixmap(wis_xpm);
            break;
        case A_DEX:
            name_label->setText("DEX: ");
            icon = QPixmap(dex_xpm);
            break;
        case A_CON:
            name_label->setText("CON: ");
            icon = QPixmap(cns_xpm);
            break;
        case A_CHA:
            name_label->setText("CHA: ");
            icon = QPixmap(cha_xpm);
            break;
    }
    QLabel *icon_label = new QLabel();
    icon_label->setPixmap(icon);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(icon_label);
    QHBoxLayout *sublayout = new QHBoxLayout();
    sublayout->setContentsMargins(0, 0, 0, 0);
    sublayout->setSpacing(0);
    sublayout->addWidget(name_label);
    sublayout->addWidget(label);
    layout->addLayout(sublayout);
}

void StatusWidget::update_value(int newval)
{
    if (value == newval)
        return;

    if (value > newval) {
        // label to red
        timeout = 5;
    } else if (value < newval) {
        // label to green
        timeout = 5;
    }
    value = newval;
    label->setText(QString::number(newval));
}

void StatusWidget::decrease_timeout()
{
    if (timeout > 1) {
        timeout--;
    } else if (timeout == 1) {
        timeout--;
        // clear label style
    }
}

NHStatusWindow::NHStatusWindow(QWidget *parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setTitleBarWidget(new QWidget(this));


    QHBoxLayout *attr_layout = new QHBoxLayout();
    attr_layout->setSpacing(20);
    str_widget = new StatusWidget(A_STR, this);
    attr_layout->addWidget(str_widget);
    intl_widget = new StatusWidget(A_INT, this);
    attr_layout->addWidget(intl_widget);
    wis_widget = new StatusWidget(A_WIS, this);
    attr_layout->addWidget(wis_widget);
    dex_widget = new StatusWidget(A_DEX, this);
    attr_layout->addWidget(dex_widget);
    con_widget = new StatusWidget(A_CON, this);
    attr_layout->addWidget(con_widget);
    cha_widget = new StatusWidget(A_CHA, this);
    attr_layout->addWidget(cha_widget);

    QHBoxLayout *variables_layout = new QHBoxLayout();
    variables_layout->setSpacing(15);
    presentation_label = new QLabel(this);
    variables_layout->addWidget(presentation_label);
    gold_label = new QLabel(this);
    variables_layout->addWidget(gold_label);
    hp_label = new QLabel(this);
    variables_layout->addWidget(hp_label);
    power_label = new QLabel(this);
    variables_layout->addWidget(power_label);
    ac_label = new QLabel(this);
    variables_layout->addWidget(ac_label);
    level_label = new QLabel(this);
    variables_layout->addWidget(level_label);
    exp_label = new QLabel(this);
    variables_layout->addWidget(exp_label);
    time_label = new QLabel(this);
    variables_layout->addWidget(time_label);

    QVBoxLayout *attr_variables_layout = new QVBoxLayout();
    attr_variables_layout->addLayout(attr_layout);
    attr_variables_layout->addLayout(variables_layout);

    QHBoxLayout *flags_layout = new QHBoxLayout();

    QHBoxLayout *statuses_layout = new QHBoxLayout();
    statuses_layout->addLayout(attr_variables_layout);
    QFrame *vline = new QFrame(this);
    vline->setFrameShape(QFrame::VLine);
    statuses_layout->addWidget(vline);
    statuses_layout->addLayout(flags_layout);
    statuses_layout->addStretch(1);

    presentation_label = new QLabel("Urist McRogue - Dungeons of doom", this);
    presentation_label->setAlignment(Qt::AlignCenter);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(presentation_label);
    layout->addLayout(statuses_layout);

    setWidget(widget);
}

void NHStatusWindow::update_presentation()
{
     QString playername(plname);
     QString role;
     if (Upolyd) {
         role = QString(mons[u.umonnum].mname);
     } else {
         role = QString(rank_of(u.ulevel, Role_switch, flags.female));
     }
     QString branch;

     if (Is_knox(&u.uz))
         branch = QString(dungeons[u.uz.dnum].dname);
     else if (In_quest(&u.uz))
         branch = QString("Home ") + QString::number(dunlev(&u.uz));
     else if (In_endgame(&u.uz))
         branch = QString(Is_astralevel(&u.uz) ? "Astral Plane" : "End Game");
     else {
         branch = QString("The Dungeons of Doom, level ") + QString::number(::depth(&u.uz));
     }

     QString new_presentation;
     QTextStream(&new_presentation) << capitalize(playername) << " the " << capitalize(role) << " - " << branch;

     presentation_label->setText(new_presentation);
}

void NHStatusWindow::update_gold()
{
    gold_label->setText(QString("$: ") + QString::number(money_cnt(invent)));
}

void NHStatusWindow::update_hp()
{
    int hp = Upolyd ? u.mh : u.uhp;
    if (hp < 0)
        hp = 0;
    int hpmax = Upolyd ? u.mhmax : u.uhpmax;
    QString new_hp;
    QTextStream(&new_hp) << "HP: " << hp << "/" << hpmax;
    hp_label->setText(new_hp);
}

void NHStatusWindow::update_power()
{
    QString new_power;
    QTextStream(&new_power) << "Pow: " << u.uen << "/" << u.uenmax;
    power_label->setText(new_power);
}

void NHStatusWindow::update_ac()
{
    ac_label->setText(QString("AC: ") + QString::number(u.uac));
}

void NHStatusWindow::update_level()
{
    if (Upolyd)
        level_label->setText(QString("HD: ") + QString::number(mons[u.umonnum].mlevel));
    else
        level_label->setText(QString("Lvl: ") + QString::number(u.ulevel));
}

void NHStatusWindow::update_exp()
{
    if (flags.showexp)
        exp_label->setText(QString("Exp: ") + QString::number(u.uexp));
    else
        exp_label->setText("");
}

void NHStatusWindow::update_time()
{
    if (flags.time)
        time_label->setText(QString("Time: ") + QString::number(moves));
    else
        time_label->setText("");
}


void NHStatusWindow::update_status()
{
    str_widget->update_value(ACURR(A_STR));
    intl_widget->update_value(ACURR(A_INT));
    wis_widget->update_value(ACURR(A_WIS));
    dex_widget->update_value(ACURR(A_DEX));
    con_widget->update_value(ACURR(A_CON));
    cha_widget->update_value(ACURR(A_CHA));
    update_presentation();
    update_gold();
    update_hp();
    update_power();
    update_ac();
    update_level();
    update_exp();
    update_time();
}
