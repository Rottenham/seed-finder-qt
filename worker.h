#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <QObject>
#include <future>

const long long inv = 2083697005;
const long long SEED_TOTAL = 0xFFFFFFFF;

enum zombie {
    REGULAR,
    CONE,
    POLE,
    BUCKET,
    NEWS,
    DOOR,
    FOOTBALL,
    DANCE,
    SNORKEL,
    ZOMBONI,
    DOLPHIN,
    JACK,
    BALLOON,
    DIGGER,
    POGO,
    BUNGEE,
    LADDER,
    CATAPULT,
    GARG,
    GIGA
};

struct SeedInfo {
    int count = 0;                // 符合标准的关卡出现了多少次
    std::vector<uint32_t> seeds;  // 符合条件的出怪数
    int seed_count = 0;           // 总共符合条件的种子数

    SeedInfo(int c, const std::vector<uint32_t> &s, int sc) : count(c), seeds(s), seed_count(sc) {}

    bool operator<(const SeedInfo &other) const
    {
        return (count < other.count);
    }
};

class SeedCalc : public QObject
{
    Q_OBJECT
public:
    std::vector<std::array<bool, 20>> view_detail();
    SeedCalc(int uid, int mode, int scene, int level_start, int level_end, uint32_t seed_start,
             uint64_t seed_end, int mask, int output_size, QObject *parent = nullptr)
        : QObject(parent)
    {
        this->uid = uid;
        this->mode = mode;
        this->scene = scene;
        this->level_start = level_start;
        this->level_end = level_end;
        this->seed_start = seed_start;
        this->seed_end = seed_end;
        this->mask = mask;
        this->output_size = output_size;
    }
    SeedCalc(int uid, int mode, int scene, int level_start, int level_end, uint32_t seed, QObject *parent = nullptr)
        : QObject(parent)
    {
        this->uid = uid;
        this->mode = mode;
        this->scene = scene;
        this->level_start = level_start;
        this->level_end = level_end;
        this->seed = seed;
    }
signals:
    void progress_updated(int val);
    void output_result(std::vector<SeedInfo> result);
public slots:
    void calc();
private:
    void calc_thread(uint32_t seed_start, int step, int code);
    void add_seed_info(uint32_t seed, int count);
    int uid, mode, level_start, level_end, scene, mask, output_size, seed = 0;
    uint32_t seed_start = 0;
    uint64_t seed_end = 0;
    std::vector<SeedInfo> result;
    std::mutex mtx;
};

#endif // WORKER_H
