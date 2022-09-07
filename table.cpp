#include "table.h"
#include "ui_table.h"
#include "worker.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QLabel>
#include <unordered_set>
#include <sstream>

Table::Table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table)
{
    //ui->setupUi(this);
    table = new QTableWidget(this);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    m_label = new QLabel;
    QFont font("Microsoft Yahei");
    font.setStyleHint(QFont::SansSerif);
    font.setPointSize(10);
    m_label->setFont(font);
    main_layout = new QGridLayout(this);
    main_layout->addWidget(m_label, 0, 0, 1, 1, Qt::AlignCenter);
    main_layout->addWidget(table, 1, 0, 1, 1, Qt::AlignCenter);
    move_to_screen_center(885, 600);
}

Table::~Table()
{
    delete ui;
}

void Table::move_to_screen_center(int expected_width, int expected_height)
{
    QDesktopWidget *desktop = QApplication::desktop();
    move((desktop->width() - expected_width) / 2, (desktop->height() - expected_height) / 2);
}

std::vector<int> get_show_list(int scene)
{
    std::unordered_set<int> exclude_lst = {REGULAR};
    if (scene != 2 && scene != 3 && scene != 7) {
        exclude_lst.insert(SNORKEL);
        exclude_lst.insert(DOLPHIN);
    }
    if (scene == 1) {
        exclude_lst.insert(ZOMBONI);
    }
    if (scene == 4 || scene == 5) {
        exclude_lst.insert(DIGGER);
        exclude_lst.insert(DANCE);
    }
    std::vector<int> lst;
    std::vector<int> preferred_order = {0, 1, 2, 3, 4, 5, 7, 8, 10, 13, 14, 15, 16, 17, 6, 9, 11, 12, 18, 19};
    for (auto l : preferred_order) {
        if (std::find(exclude_lst.begin(), exclude_lst.end(), l) != exclude_lst.end()) continue;
        lst.push_back(l);
    }
    return lst;
}

void Table::update_contents(std::vector<std::array<bool, 20>> data, int start_flags, int scene, std::vector<bool> zombie_flags, bool bring_to_front)
{
    auto list = get_show_list(scene);
    table->clearContents();
    table->setColumnCount(list.size() + 1);
    table->setRowCount(data.size());
    std::cout << bring_to_front;

    // contents of table
    int hard_level_count = 0;
    for (size_t r = 0; r < data.size(); ++r) {
        auto item = new QTableWidgetItem(QString::number(start_flags));
        item->setTextAlignment(Qt::AlignCenter);
        table->setItem(r, 0, item);
        start_flags += 2;
        for (size_t i = 0; i < list.size(); ++i) {
            if (data.at(r).at(list.at(i))) {
                auto item_2 = new QTableWidgetItem(QString::fromLocal8Bit("○"));
                item_2->setTextAlignment(Qt::AlignCenter);
                table->setItem(r, i + 1, item_2);
            }
        }
        bool is_hard = true;
        for (int i = 1; i <= GIGA; ++i) {
            if (zombie_flags.at(i) && !data.at(r).at(i)) {
                is_hard = false;
                break;
            }
        }
        if (is_hard) ++hard_level_count;
    }
    std::ostringstream oss;
    oss << "共" << data.size() << "次选卡, " << hard_level_count << "个难度关";
    m_label->setText(QString::fromLocal8Bit(oss.str().c_str()));

    // set headers
    QStringList hhl;
    hhl << QString::fromLocal8Bit("已完成f数");
    for (auto &&l : list)
        hhl << QString::fromLocal8Bit(zombie_names.at(l));
    table->setHorizontalHeaderLabels(hhl);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // set size to fit
    table->resizeColumnsToContents();
    table->resizeRowsToContents();

    if (this->isVisible()) {
        if (bring_to_front) {
            this->activateWindow();
            this->raise();
        }
    } else {
        this->show();
    }
    this->adjustSize();
}
