#include "qt5_port.h"
#include "onames.h"
#include <QFrame>
#include <QGroupBox>

NHNameDialog::NHNameDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(10, 10, 10, 10);
    main_layout->setSpacing(5);

// XXX this is unfortunately never used by unixmain
#ifdef SELECTSAVED
    if (iflags.wc2_selectsaved && !iflags.renameinprogress) {
        char **saved_games =  get_saved_games();
        bool has_saved_games = saved_games && (saved_games[0] != nullptr);

        if (has_saved_games) {
            QGroupBox *saved_games_groupbox = new QGroupBox(this);
            saved_games_groupbox->setTitle("Saved Games");

            QVBoxLayout *saved_games_layout = new QVBoxLayout();

            int i = 0;
            while (saved_games[i] != nullptr) {
                QPushButton *saved_game_button = new QPushButton(QString(saved_games[i]), saved_games_groupbox);
                saved_game_button->setObjectName(QString(saved_games[i]));
                connect(saved_game_button, &QPushButton::clicked, this, &NHNameDialog::load_game);
                saved_games_layout->addWidget(saved_game_button);
                i++;
            }
            saved_games_groupbox->setLayout(saved_games_layout);

            main_layout->addWidget(saved_games_groupbox);
            main_layout->addStretch(1);
        }

        free_saved_games(saved_games);
    }
#endif

    QGroupBox *name_groupbox = new QGroupBox(this);
    name_groupbox->setTitle("Player name");

    QHBoxLayout *editor_layout = new QHBoxLayout();
    editor_layout->setSpacing(10);

    editor = new QLineEdit(name_groupbox);
    connect(editor, &QLineEdit::textChanged, this, &NHNameDialog::on_editor_changed);
    editor_accept = new QPushButton("Ok", name_groupbox);
    editor_accept->setFocus();
    editor_accept->setDisabled(true);
    connect(editor_accept, &QPushButton::clicked, this, &NHNameDialog::accept);
    editor_layout->addWidget(editor);
    editor_layout->addWidget(editor_accept);

    name_groupbox->setLayout(editor_layout);
    main_layout->addWidget(name_groupbox);

    QHBoxLayout *cancel_layout = new QHBoxLayout();
    cancel_layout->setContentsMargins(0, 0, 0, 0);
    QPushButton *reject_button = new QPushButton("Cancel", this);
    connect(reject_button, &QPushButton::clicked, this, &NHNameDialog::reject);
    cancel_layout->addWidget(reject_button);
    cancel_layout->addStretch(1);

    main_layout->addLayout(cancel_layout);
}

void NHNameDialog::on_editor_changed()
{
    editor_accept->setDisabled(editor->text().length() == 0);
}

void NHNameDialog::load_game()
{
    QObject *sender_btn = sender();
    editor->setText(sender_btn->objectName());

    accept();
}

QString NHNameDialog::get_name()
{
    return editor->text();
}
