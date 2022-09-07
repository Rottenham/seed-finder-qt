#include "setting.h"
#include "ui_setting.h"
#include "worker.h"
#include <QIntValidator>

Setting::Setting(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    // set up config
    for (int i = 0; i < GIGA; ++i) {
        _zombie_flags.push_back(false);
    }
    _zombie_flags.push_back(true);

    // valiate
    ui->seed_total_val->setValidator(new QIntValidator(1, inv, this));
}

Setting::~Setting()
{
    delete ui;
}

void Setting::show_setting()
{
    ui->seed_total_val->setText(QString::number(_seed_span));
}
