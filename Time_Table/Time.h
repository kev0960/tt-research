#include <bitset>
#include <cstdint>
#pragma once

using namespace std;

int start_bit(bitset<32> time);
void set_time(bitset<32> &t, int time);
bool time_avail(bitset<32> &t, int time);

inline bool time_avail(bitset<32> &t, bitset<32> time);
void next_time(bitset<32> &time);