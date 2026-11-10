// 全部 19 角色符卡名（plXX.ecl opcode522 SSSx XOR 加密字符串，thtk thecl10.c 格式表）
// 每个角色：主符卡名数组，空=无符卡。UTF-8。
#pragma once
#include <stdint.h>
struct SpellCardList { const char* const* names; int count; };
static const char* const PL00_CARDS[] = {
    "霊符「乱撃陰陽玉」",
    "「在庫全ての陰陽玉」",
    "「鬼畜生調伏に慈悲は無し」",
    nullptr
};
static const char* const PL01_CARDS[] = {
    "星符「シュート・ザ・ビースト」",
    nullptr
};
static const char* const PL02_CARDS[] = {
    "風符「ウィンドライダー」",
    "風符「ウィンドマスター」",
    nullptr
};
static const char* const PL03_CARDS[] = {
    "式神「スティルハンター　チェン」",
    "式神「マルチスティルハンター」",
    nullptr
};
static const char* const PL04_CARDS[] = {
    "独楽「コマ犬回し」",
    "独楽「コマ犬大回転」",
    nullptr
};
static const char* const PL05_CARDS[] = {
    "宝石「ジュエルレイン」",
    nullptr
};
static const char* const PL06_CARDS[] = {
    "弾符「ラビットシューティング」",
    nullptr
};
static const char* const PL07_CARDS[] = {
    "死霊「フェアリーオブザデッド」",
    nullptr
};
static const char* const PL08_CARDS[] = {
    "管狐「シリンダーフォックス」",
    nullptr
};
static const char* const PL09_CARDS[] = {
    "狸符「強欲な動物霊に紛れよ！」",
    nullptr
};
static const char* const PL10_CARDS[] = {
    "亀符「亀甲地獄」",
    "亀符「吉弔大結界」",
    "龍符「龍紋弾」",
    nullptr
};
static const char* const PL11_CARDS[] = {
    "天星馬「ペガサスクロス」",
    "天星馬「ペガサススターマイン」",
    "「ラストシュート」",
    nullptr
};
static const char* const PL12_CARDS[] = {
    "宿怨「ゴージライザー」",
    "「アブソーバーファントム」",
    "「底辺の暴食」",
    nullptr
};
static const char* const PL13_CARDS[] = {
    "鬼符「濛々酒鬼」",
    "鬼符「疎音濛々酒鬼」",
    "「美しき地獄弾幕夜行」",
    nullptr
};
static const char* const PL14_CARDS[] = {
    "猿撃「モンキーマジック」",
    nullptr
};
static const char* const PL15_CARDS[] = {
    "三頭「ケルベロスファイア」",
    nullptr
};
static const char* const PL16_CARDS[] = {
    "呪血「カースドデビル」",
    nullptr
};
static const char* const PL17_CARDS[] = {
    "執行者「脱獄ストーカー」",
    nullptr
};
static const char* const PL18_CARDS[] = {
    "「純霊弾」",
    "「無心純霊弾」",
    "「亡羊のキングダム」",
    nullptr
};
static const SpellCardList SPELL_CARDS[19] = {
    { PL00_CARDS, 3 },
    { PL01_CARDS, 1 },
    { PL02_CARDS, 2 },
    { PL03_CARDS, 2 },
    { PL04_CARDS, 2 },
    { PL05_CARDS, 1 },
    { PL06_CARDS, 1 },
    { PL07_CARDS, 1 },
    { PL08_CARDS, 1 },
    { PL09_CARDS, 1 },
    { PL10_CARDS, 3 },
    { PL11_CARDS, 3 },
    { PL12_CARDS, 3 },
    { PL13_CARDS, 3 },
    { PL14_CARDS, 1 },
    { PL15_CARDS, 1 },
    { PL16_CARDS, 1 },
    { PL17_CARDS, 1 },
    { PL18_CARDS, 3 },
};
