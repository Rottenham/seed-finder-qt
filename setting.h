#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();
    void show_setting();

    // config
    int _output_size = 5;
    bool _start_at_random_seed = false;
    int _seed_start = 0;
    int _seed_span = 1000;
    std::vector<bool> _zombie_flags;

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
