﻿#include "worker.h"
#include <iostream>
#include <random>
#include <thread>
#include <array>
#include <bitset>

class d_page
{
public:
    static const int LENGTH = 10;
    static const int BLOCK = (1 << LENGTH);
    static const int MASK = (BLOCK - 1);

private:
    class page
    {
        int id;
        int scene;
        int data[BLOCK][8];

        const int allow_base[8] = {
            0x0f0ffaff, 0x0f0ff8ff, 0x0f0fffff, 0x0f0fffff,
            0x0f0fda7f, 0x0f0fda7f, 0x0f0ffaff, 0x0f0ffaff
        };
        const int conv_id[33] = {
            31, 24, 1, 2, 3,
            4, 5, 6, 7, 31,
            31, 8, 9, 31, 10,
            11, 12, 13, 14, 31,
            15, 16, 17, 18, 31, 25,
            31, 31, 31, 31, 31, 31,
            19
        };
        uint32_t buf[0x270];
        int idx;
        uint32_t rng_gen(int max)
        {
            uint32_t tmp;

            tmp = (buf[idx] & 0x80000000) | (buf[idx + 1] & 0x7FFFFFFF);
            buf[idx] = (tmp >> 1) ^ buf[(idx + 0x18D) % 0x270];
            if (tmp & 1) {
                buf[idx] ^= 0x9908B0DF;
            }

            buf[idx + 0x18D + 1] = (buf[idx + 0x18D] ^ (buf[idx + 0x18D] >> 30)) * 0x6C078965 + idx + 0x18D + 1;

            tmp = buf[idx];
            tmp ^= (tmp >> 11);
            tmp ^= ((tmp & 0xFF3A58AD) << 7);
            tmp ^= ((tmp & 0xFFFFDF8C) << 15);
            tmp ^= (tmp >> 18);
            idx = idx + 1;
            return (tmp & 0x7FFFFFFF) % max;
        }
        void rng_init(uint32_t seed)
        {
            buf[0] = seed;
            for (int i = 1; i < 0x18e; ++i) {
                buf[i] = (buf[i - 1] ^ (buf[i - 1] >> 30)) * 0x6C078965 + i;
            }
            idx = 0;
        }

    public:
        void init(int sc, int idx)
        {
            int d1[10];
            int allow;
            scene = sc;
            id = idx;
            allow = allow_base[scene];
            uint32_t cur_seed = id * BLOCK;
            for (int i = 0; i < BLOCK; i++) {
                uint32_t rng_seed = cur_seed * 101;
                if (rng_seed == 0) rng_seed = 0x1105;
                rng_init(rng_seed);
                cur_seed += 1;
                int has = 1;
                d1[0] = 1;
                if (rng_gen(5) == 0) {
                    d1[0] |= (1 << NEWS);
                    has |= (1 << NEWS);
                } else {
                    d1[0] |= (1 << CONE);
                    has |= (1 << CONE);
                }
                int j = 1;
                while (j < 10) {
                    int type = rng_gen(33);
                    int nid = conv_id[type];
                    if (has & (1 << nid)) continue;
                    if (allow & (1 << nid)) {
                        has |= (1 << nid);
                        d1[j] = d1[j - 1];
                        if (nid != 24 && nid != 25) d1[j] |= (1 << nid);
                        j++;
                    }
                }
                for (j = 2; j < 6; j++) {
                    if (d1[j] & (1 << GIGA)) d1[j] = d1[j + 1] ^ (1 << GIGA);
                }
                if (d1[2] & (1 << ZOMBONI)) {
                    if (d1[3] & (1 << GARG)) {
                        d1[2] = d1[4] ^ (1 << ZOMBONI) ^ (1 << GARG);
                    } else {
                        d1[2] = d1[3] ^ (1 << ZOMBONI);
                    }
                } else if (d1[2] & (1 << GARG)) {
                    if (d1[3] & (1 << ZOMBONI)) {
                        d1[2] = d1[4] ^ (1 << ZOMBONI) ^ (1 << GARG);
                    } else {
                        d1[2] = d1[3] ^ (1 << GARG);
                    }
                }
                for (j = 0; j < 8; j++) {
                    data[i][j] = d1[j + 2];
                }
            }
        }
        int get(uint32_t pos, int level)
        {
            return data[pos][level];
        }
    };
    page p0, p1;
    int cur;
    int scene;

public:
    d_page() {}
    d_page(int sc)
    {
        scene = sc;
        p0.init(sc, 0);
        p1.init(sc, 1);
    }
    int get(uint32_t pos, int level)
    {
        if (level == 0) return 11;
        level--;
        if (level >= 8) level = 7;
        int t = int(pos >> LENGTH);
        if (t == cur + 2) {
            if (cur & 1) {
                p1.init(scene, cur + 2);
            } else {
                p0.init(scene, cur + 2);
            }
            cur++;
        }
        if (t == cur - 1) {
            if (cur & 1) {
                p1.init(scene, cur - 1);
            } else {
                p0.init(scene, cur - 1);
            }
            cur--;
        }
        if (t != cur && t != cur + 1) {
            if (t & 1) {
                p1.init(scene, t);
                p0.init(scene, t + 1);
            } else {
                p1.init(scene, t + 1);
                p0.init(scene, t);
            }
            cur = t;
        }
        if (t & 1) {
            return p1.get(pos & MASK, level);
        } else {
            return p0.get(pos & MASK, level);
        }
    }
};

int single_seed(uint32_t seed, int mask, int uid, int mode, int scene, int begin, int end)
{
    d_page p1(scene);
    uint32_t l = (seed + uid + mode) * inv + begin;
    int count = 0;
    for (int i = begin; i < end; i++, l++) {
        int t = p1.get(l, i);
        if ((t & mask) == mask) ++count;
    }
    return count;
}

void SeedCalc::add_seed_info(uint32_t seed, int count)
{
    mtx.lock();
    bool found = false;
    for (auto &info : result) {
        if (info.count == count) {
            found = true;
            ++info.seed_count;
            if (info.seed_count <= output_size)
                info.seeds.push_back(seed);
            break;
        }
    }
    if (!found) {
        result.push_back(SeedInfo(count, {seed}, 1));
    }
    mtx.unlock();
}

void SeedCalc::calc_thread(uint32_t seed_start, int step)
{
    uint32_t seed = seed_start;
    while (!stopThread && seed < seed_end) {
        int count = single_seed(seed, mask, uid, mode, scene, level_start, level_end);
        add_seed_info(seed, count);
        seed += step;
    }
}

std::vector<SeedInfo> SeedCalc::calc()
{
    result.clear();
    int num = std::thread::hardware_concurrency();
    std::cout << "thread num = " << num << "\n";
    std::vector<std::thread> threadList;
    for (int i = 0; i < num; ++i) {
        threadList.push_back(std::thread(&SeedCalc::calc_thread, this, i + seed_start, num));
    }
    for (int i = 0; i < num; ++i) {
        threadList[i].join();
    }
    return result;
}

std::vector<std::array<bool, 20>> SeedCalc::view_detail()
{
    std::vector<std::array<bool, 20>> result;
    d_page p1(scene);
    uint32_t l = (seed + uid + mode) * inv + level_start;
    for (int i = level_start; i < level_end; ++i, ++l) {
        int t = p1.get(l, i);
        std::array<bool, 20> row;
        for (int z = REGULAR; z <= GIGA; ++z) {
            row[z] = t & 1;
            t >>= 1;
        }
        result.push_back(row);
    }
    return result;
}
