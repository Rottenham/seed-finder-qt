#ifndef SEEDFINDER_H
#define SEEDFINDER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SeedFinder; }
QT_END_NAMESPACE

#define VERSION_NAME_FULL "1.1"

class SeedFinder : public QMainWindow
{
    Q_OBJECT

public:
    SeedFinder(QWidget *parent = nullptr);
    ~SeedFinder();

private slots:
    void on_detailedSetting_triggered();
    void on_howToUse_triggered();

    void on_about_triggered();

    void on_start_flags_textChanged(const QString &arg1);

    void on_flags_span_textEdited(const QString &arg1);

    void on_hardness_textEdited(const QString &arg1);

    void on_hardness_slider_valueChanged(int value);

    void on_calc_toggled(bool checked);

    void on_detail_clicked();

private:
    void show_child_window(QWidget *dialog);
    void update_end_flags();
    void set_all_widgets_availability(bool flag);
    void show_warning(const char *title, const char *msg);
    int get_scene();
    bool check_inputs();
    bool check_current_seed();
    int get_mask();
    void show_detail(bool bring_to_front);
    Ui::SeedFinder *ui;
    class Setting *setting_page;
    class HowTo *howto_page;
    class About *about_page;
    class Table *table_page;

};
#endif // SEEDFINDER_H
