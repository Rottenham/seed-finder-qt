#include "setting.h"
#include "ui_setting.h"
#include "worker.h"
#include <QIntValidator>
#include <QMessageBox>
#include <sstream>
#include <iomanip>

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
    QRegExp rx_10_digit("[1-9]\\d{0,9}");
    ui->seed_total_val->setValidator(new QRegExpValidator(rx_10_digit, this));
    QRegExp rx_hex_digit("[0-9a-fA-F]{8}");
    ui->seed_logic_val->setValidator(new QRegExpValidator(rx_hex_digit, this));
    QRegExp rx_2_digit("[1-9]\\d{0,1}");
    ui->output_size->setValidator(new QRegExpValidator(rx_2_digit, this));
}

Setting::~Setting()
{
    delete ui;
}

void Setting::show_setting()
{
    ui->seed_total_val->setText(QString::number(_seed_span));
    ui->seed_logic_rnd->setChecked(_start_at_random_seed);
    ui->seed_logic_fxd->setChecked(!_start_at_random_seed);
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << _seed_start;
    ui->seed_logic_val->setText(QString::fromStdString(ss.str()));
    ui->output_size->setText(QString::number(_output_size));
    for (int i = 1; i <= 19; ++i)
        this->findChild<QCheckBox *>("checkBox_" + QString::number(i))->setChecked(_zombie_flags.at(i));
}

void Setting::show_warning(const char *title, const char *msg)
{
    QMessageBox Msgbox(this);
    Msgbox.setWindowTitle(QString::fromLocal8Bit(title));
    Msgbox.setIcon(QMessageBox::Information);
    Msgbox.setText(QString::fromLocal8Bit(msg));
    Msgbox.exec();
}

void Setting::on_submit_clicked()
{
    if (ui->seed_total_val->text().isEmpty()) {
        show_warning("输入错误", "寻种总数为空");
        ui->seed_total_val->setFocus();
        return;
    }
    if (ui->seed_logic_fxd->isChecked() && ui->seed_logic_val->text().isEmpty()) {
        show_warning("输入错误", "寻种起始值为空");
        ui->seed_logic_val->setFocus();
        return;
    }
    if (ui->output_size->text().isEmpty()) {
        show_warning("输入错误", "输出种子数为空");
        ui->output_size->setFocus();
        return;
    }
    bool at_least_one_checked = false;
    for (int i = 1; i <= 19; ++i)
        if (this->findChild<QCheckBox *>("checkBox_" + QString::number(i))->isChecked()) {
            at_least_one_checked = true;
            break;
        }
    if (!at_least_one_checked) {
        show_warning("输入错误", "未选择任何僵尸");
        return;
    }

    // 检查种子开始+种子数是否溢出
    uint32_t logic_val = ui->seed_logic_rnd->isChecked() ? 0 :
                         std::stoul(ui->seed_logic_val->text().toStdString(), nullptr, 16);
    qulonglong t = ui->seed_total_val->text().toULongLong() + logic_val;
    bool skip = false;
    if (t > SEED_TOTAL + 1) {
        qulonglong new_val = SEED_TOTAL + 1 - logic_val;
        ui->seed_total_val->setText(QString::number(new_val));
        _seed_span = new_val;
        skip = true;
        std::ostringstream ss;
        ss << "寻种范围超出上限，已自动调整寻种总数为" << new_val;
        show_warning("提示", ss.str().c_str());
    }

    // 设置
    if (!skip) _seed_span = ui->seed_total_val->text().toInt();
    _start_at_random_seed = ui->seed_logic_rnd->isChecked();
    if (!_start_at_random_seed) _seed_start = logic_val;
    _output_size = ui->output_size->text().toInt();
    for (int i = 1; i <= 19; ++i)
        _zombie_flags[i] = this->findChild<QCheckBox *>("checkBox_" + QString::number(i))->isChecked();
    this->close();
    std::cout << _seed_span;
}
