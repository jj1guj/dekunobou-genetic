#include<algorithm>
#include<chrono>
#include<iostream>
#include<limits>
#include<random>
#include<cstring>
#include<string>
#include<vector>
#define param_size 60
#define cur_opening 0
#define cur_middle 20
#define cur_ending 40
#define inf 1<<30
#define ll long long

extern int cur_offset;//序盤・中盤・終盤でパラメータの配列のどのインデックスから値を引っ張ってくればいいか計算する