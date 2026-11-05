// Generated from rueee/thprac master_2, 1778005 (MIT).
// TH17 source SHA-256 3db9c33b1f4760d5082eb25dc117cf06a95a83f759e3e4ca3dc4d0c2334bcbd0
#pragma once
namespace th17 {
enum PracticeSection {
    TH17_ST1_MID1 = 1,
    TH17_ST1_BOSS1 = 2,
    TH17_ST1_BOSS2 = 3,
    TH17_ST1_BOSS3 = 4,
    TH17_ST1_BOSS4 = 5,
    TH17_ST2_MID1 = 6,
    TH17_ST2_BOSS1 = 7,
    TH17_ST2_BOSS2 = 8,
    TH17_ST2_BOSS3 = 9,
    TH17_ST2_BOSS4 = 10,
    TH17_ST2_BOSS5 = 11,
    TH17_ST3_MID1 = 12,
    TH17_ST3_BOSS1 = 13,
    TH17_ST3_BOSS2 = 14,
    TH17_ST3_BOSS3 = 15,
    TH17_ST3_BOSS4 = 16,
    TH17_ST3_BOSS5 = 17,
    TH17_ST3_BOSS6 = 18,
    TH17_ST4_MID1 = 19,
    TH17_ST4_BOSS1 = 20,
    TH17_ST4_BOSS2 = 21,
    TH17_ST4_BOSS3 = 22,
    TH17_ST4_BOSS4 = 23,
    TH17_ST4_BOSS5 = 24,
    TH17_ST4_BOSS6 = 25,
    TH17_ST5_MID1 = 26,
    TH17_ST5_BOSS1 = 27,
    TH17_ST5_BOSS2 = 28,
    TH17_ST5_BOSS3 = 29,
    TH17_ST5_BOSS4 = 30,
    TH17_ST5_BOSS5 = 31,
    TH17_ST5_BOSS6 = 32,
    TH17_ST5_BOSS7 = 33,
    TH17_ST6_MID1 = 34,
    TH17_ST6_BOSS1 = 35,
    TH17_ST6_BOSS2 = 36,
    TH17_ST6_BOSS3 = 37,
    TH17_ST6_BOSS4 = 38,
    TH17_ST6_BOSS5 = 39,
    TH17_ST6_BOSS6 = 40,
    TH17_ST6_BOSS7 = 41,
    TH17_ST6_BOSS8 = 42,
    TH17_ST6_BOSS9 = 43,
    TH17_ST6_BOSS10 = 44,
    TH17_ST6_BOSS11 = 45,
    TH17_ST7_MID1 = 46,
    TH17_ST7_MID2 = 47,
    TH17_ST7_MID3 = 48,
    TH17_ST7_END_NS1 = 49,
    TH17_ST7_END_S1 = 50,
    TH17_ST7_END_NS2 = 51,
    TH17_ST7_END_S2 = 52,
    TH17_ST7_END_NS3 = 53,
    TH17_ST7_END_S3 = 54,
    TH17_ST7_END_NS4 = 55,
    TH17_ST7_END_S4 = 56,
    TH17_ST7_END_NS5 = 57,
    TH17_ST7_END_S5 = 58,
    TH17_ST7_END_NS6 = 59,
    TH17_ST7_END_S6 = 60,
    TH17_ST7_END_NS7 = 61,
    TH17_ST7_END_S7 = 62,
    TH17_ST7_END_NS8 = 63,
    TH17_ST7_END_S8 = 64,
    TH17_ST7_END_S9 = 65,
    TH17_ST7_END_S10 = 66,
};
struct PracticeSectionInfo {
    int id, stage, group, order, bgm;
    bool spell, dialogue, drops;
    const char* names[5][3];
};
inline constexpr PracticeSectionInfo practice_sections[] = {
    {TH17_ST1_MID1, 1, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST1_BOSS1, 1, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST1_BOSS2, 1, 2, 2, 1, true, false, true, {
        {"石符「石林」", "Stone Sign \"Stone Woods\"", "石符「ストーンウッズ」"}, {"石符「石林」", "Stone Sign \"Stone Woods\"", "石符「ストーンウッズ」"}, {"石符「石头针叶林」", "Stone Sign \"Stone Conifers\"", "石符「ストーンコニファー」"}, {"石符「石头针叶林」", "Stone Sign \"Stone Conifers\"", "石符「ストーンコニファー」"}, {"", "", ""}}},
    {TH17_ST1_BOSS3, 1, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST1_BOSS4, 1, 2, 4, 1, true, false, true, {
        {"石符「儿童们的灵薄狱」", "Stone Sign \"Children's Limbo\"", "石符「チルドレンズリンボ」"}, {"石符「儿童们的灵薄狱」", "Stone Sign \"Children's Limbo\"", "石符「チルドレンズリンボ」"}, {"石符「成年儿童们的灵薄狱」", "Stone Sign \"Adult Children's Limbo\"", "石符「アダルトチルドレンズリンボ」"}, {"石符「成年儿童们的灵薄狱」", "Stone Sign \"Adult Children's Limbo\"", "石符「アダルトチルドレンズリンボ」"}, {"", "", ""}}},
    {TH17_ST2_MID1, 2, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST2_BOSS1, 2, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST2_BOSS2, 2, 2, 2, 1, true, false, true, {
        {"石符「石之婴儿」", "Stone Sign \"Stone Baby\"", "石符「ストーンベイビー」"}, {"石符「石之婴儿」", "Stone Sign \"Stone Baby\"", "石符「ストーンベイビー」"}, {"石符「沉重的石之婴儿」", "Stone Sign \"Heavy Stone Baby\"", "石符「ヘビーストーンベイビー」"}, {"石符「沉重的石之婴儿」", "Stone Sign \"Heavy Stone Baby\"", "石符「ヘビーストーンベイビー」"}, {"", "", ""}}},
    {TH17_ST2_BOSS3, 2, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST2_BOSS4, 2, 2, 4, 1, true, false, true, {
        {"溺符「三途的沦溺」", "Drowning Sign \"Drowning in the Sanzu\"", "溺符「三途の淪溺」"}, {"溺符「三途的沦溺」", "Drowning Sign \"Drowning in the Sanzu\"", "溺符「三途の淪溺」"}, {"溺符「三途的沦溺」", "Drowning Sign \"Drowning in the Sanzu\"", "溺符「三途の淪溺」"}, {"溺符「三途的沦溺」", "Drowning Sign \"Drowning in the Sanzu\"", "溺符「三途の淪溺」"}, {"", "", ""}}},
    {TH17_ST2_BOSS5, 2, 2, 5, 1, true, false, true, {
        {"鬼符「魔鬼围城」", "Oni Sign \"Demon Siege\"", "鬼符「デーモンシージ」"}, {"鬼符「魔鬼围城」", "Oni Sign \"Demon Siege\"", "鬼符「デーモンシージ」"}, {"鬼符「饿鬼围城」", "Oni Sign \"Hungry Demon Siege\"", "鬼符「ハングリーデーモンシージ」」"}, {"鬼符「饿鬼围城」", "Oni Sign \"Hungry Demon Siege\"", "鬼符「ハングリーデーモンシージ」」"}, {"", "", ""}}},
    {TH17_ST3_MID1, 3, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST3_BOSS1, 3, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST3_BOSS2, 3, 2, 2, 1, true, false, true, {
        {"水符「分水的试练」", "Water Sign \"Ordeal of Water Distribution\"", "水符「水配りの試練」"}, {"水符「分水的试练」", "Water Sign \"Ordeal of Water Distribution\"", "水符「水配りの試練」"}, {"水符「分水的上级试炼」", "Water Sign \n\"High-Level Purgatordeal of Water Distribution\"", "水符「水配りの上級試煉」"}, {"水符「分水的顶级试炼」", "Water Sign \n\"Ultimate Purgatordeal of Water Distribution\"", "水符「水配りの極級試煉」"}, {"", "", ""}}},
    {TH17_ST3_BOSS3, 3, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST3_BOSS4, 3, 2, 4, 1, true, false, true, {
        {"光符「瞭望的试练」", "Light Sign \"Ordeal of Surveying\"", "光符「見渡しの試練」"}, {"光符「瞭望的试练」", "Light Sign \"Ordeal of Surveying\"", "光符「見渡しの試練」"}, {"光符「瞭望的上级试炼」", "Light Sign \"High-Level Purgatordeal of Surveying\"", "光符「見渡しの上級試煉」"}, {"光符「瞭望的顶级试炼」", "Light Sign \"Ultimate Purgatordeal of Surveying\"", "光符「見渡しの極級試煉」"}, {"", "", ""}}},
    {TH17_ST3_BOSS5, 3, 2, 5, 1, false, false, true, {
        {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"", "", ""}}},
    {TH17_ST3_BOSS6, 3, 2, 6, 1, true, false, true, {
        {"鬼符「鬼渡的试练」", "Oni Sign \"Purgatordeal of Oniwatari\"", "鬼符「鬼渡の試練」"}, {"鬼符「鬼渡的试练」", "Oni Sign \"Purgatordeal of Oniwatari\"", "鬼符「鬼渡の試練」"}, {"鬼符「鬼渡的上级试炼」", "Oni Sign \"High-Level Purgatordeal of Oniwatari\"", "鬼符「鬼渡の上級試煉」"}, {"鬼符「鬼渡的狱级试炼」", "Oni Sign \"Hell-Level Purgatordeal of Oniwatari\"", "鬼符「鬼渡の獄級試煉」"}, {"", "", ""}}},
    {TH17_ST4_MID1, 4, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST4_BOSS1, 4, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST4_BOSS2, 4, 2, 2, 1, true, false, true, {
        {"龟符「龟甲地狱」", "Tortoise Sign \"Tortoiseshell Hell\"", "亀符「亀甲地獄」"}, {"龟符「龟甲地狱」", "Tortoise Sign \"Tortoiseshell Hell\"", "亀符「亀甲地獄」"}, {"龟符「龟甲地狱」", "Tortoise Sign \"Tortoiseshell Hell\"", "亀符「亀甲地獄」"}, {"龟符「龟甲地狱」", "Tortoise Sign \"Tortoiseshell Hell\"", "亀符「亀甲地獄」"}, {"", "", ""}}},
    {TH17_ST4_BOSS3, 4, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST4_BOSS4, 4, 2, 4, 1, true, false, true, {
        {"鬼符「搦手的畜生」", "Oni Sign \"Beasts Attacking from the Rear\"", "鬼符「搦手の畜生」"}, {"鬼符「搦手的畜生」", "Oni Sign \"Beasts Attacking from the Rear\"", "鬼符「搦手の畜生」"}, {"鬼符「搦手的犬畜生」", "Oni Sign \"Mangy Beasts Attacking from the Rear\"", "鬼符「搦手の犬畜生」"}, {"鬼符「搦手的鬼畜生」", "Oni Sign \"Devilish Beasts Attacking from the Rear\"", "鬼符「搦手の鬼畜生」"}, {"", "", ""}}},
    {TH17_ST4_BOSS5, 4, 2, 5, 1, false, false, true, {
        {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"", "", ""}}},
    {TH17_ST4_BOSS6, 4, 2, 6, 1, true, false, true, {
        {"龙符「龙纹弹」", "Dragon Sign \"Dragon Crest Bullets\"", "龍符「龍紋弾」"}, {"龙符「龙纹弹」", "Dragon Sign \"Dragon Crest Bullets\"", "龍符「龍紋弾」"}, {"龙符「龙纹弹」", "Dragon Sign \"Dragon Crest Bullets\"", "龍符「龍紋弾」"}, {"龙符「龙纹弹」", "Dragon Sign \"Dragon Crest Bullets\"", "龍符「龍紋弾」"}, {"", "", ""}}},
    {TH17_ST5_MID1, 5, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST5_BOSS1, 5, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST5_BOSS2, 5, 2, 2, 1, true, false, true, {
        {"埴轮「弓兵埴轮」", "Haniwa \"Archer Haniwa\"", "埴輪「弓兵埴輪」"}, {"埴轮「弓兵埴轮」", "Haniwa \"Archer Haniwa\"", "埴輪「弓兵埴輪」"}, {"埴轮「熟练弓兵埴轮」", "Haniwa \"Skilled Archer Haniwa\"", "埴輪「熟練弓兵埴輪」"}, {"埴轮「熟练弓兵埴轮」", "Haniwa \"Skilled Archer Haniwa\"", "埴輪「熟練弓兵埴輪」"}, {"", "", ""}}},
    {TH17_ST5_BOSS3, 5, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST5_BOSS4, 5, 2, 4, 1, true, false, true, {
        {"埴轮「剑士埴轮」", "Haniwa \"Fencer Haniwa\"", "埴輪「剣士埴輪」"}, {"埴轮「剑士埴轮」", "Haniwa \"Fencer Haniwa\"", "埴輪「剣士埴輪」"}, {"埴轮「熟练剑士埴轮」", "Haniwa \"Skilled Fencer Haniwa\"", "埴輪「熟練剣士埴輪」"}, {"埴轮「熟练剑士埴轮」", "Haniwa \"Skilled Fencer Haniwa\"", "埴輪「熟練剣士埴輪」"}, {"", "", ""}}},
    {TH17_ST5_BOSS5, 5, 2, 5, 1, false, false, true, {
        {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"", "", ""}}},
    {TH17_ST5_BOSS6, 5, 2, 6, 1, true, false, true, {
        {"埴轮「骑马兵埴轮」", "Haniwa \"Cavalry Haniwa\"", "埴輪「騎馬兵埴輪」"}, {"埴轮「骑马兵埴轮」", "Haniwa \"Cavalry Haniwa\"", "埴輪「騎馬兵埴輪」"}, {"埴轮「熟练骑马兵埴轮」", "Haniwa \"Skilled Cavalry Haniwa\"", "埴輪「熟練騎馬兵埴輪」"}, {"埴轮「熟练骑马兵埴轮」", "Haniwa \"Skilled Cavalry Haniwa\"", "埴輪「熟練騎馬兵埴輪」"}, {"", "", ""}}},
    {TH17_ST5_BOSS7, 5, 2, 7, 1, true, false, true, {
        {"埴轮「空洞的无尽兵团」", "Haniwa \"Hollow Inexhaustible Troops\"", "埴輪「がらんどうの無尽兵団」"}, {"埴轮「空洞的无尽兵团」", "Haniwa \"Hollow Inexhaustible Troops\"", "埴輪「がらんどうの無尽兵団」"}, {"埴轮「不败的无尽兵团」", "Haniwa \"Undefeated Inexhaustible Troops\"", "埴輪「不敗の無尽兵団」"}, {"埴轮「不败的无尽兵团」", "Haniwa \"Undefeated Inexhaustible Troops\"", "埴輪「不敗の無尽兵団」"}, {"", "", ""}}},
    {TH17_ST6_MID1, 6, 1, 1, 0, false, false, false, {
        {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"道中一非", "Mid Nonspell 1", "道中 通常1"}, {"", "", ""}}},
    {TH17_ST6_BOSS1, 6, 2, 1, 1, false, true, false, {
        {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}, {"", "", ""}}},
    {TH17_ST6_BOSS2, 6, 2, 2, 1, true, false, true, {
        {"方形「方形造形术」", "Square Shape \"Square-Shaped Sculpting Art\"", "方形「方形造形術」"}, {"方形「方形造形术」", "Square Shape \"Square-Shaped Sculpting Art\"", "方形「方形造形術」"}, {"方形「方形造物」", "Square Shape \"Square Creature\"", "方形「スクエアクリーチャー」"}, {"方形「方形造物」", "Square Shape \"Square Creature\"", "方形「スクエアクリーチャー」"}, {"", "", ""}}},
    {TH17_ST6_BOSS3, 6, 2, 3, 1, false, false, true, {
        {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}, {"", "", ""}}},
    {TH17_ST6_BOSS4, 6, 2, 4, 1, true, false, true, {
        {"圆形「正圆造形术」", "Circular Shape \"Perfect Circle Sculpting Art\"", "円形「真円造形術」"}, {"圆形「正圆造形术」", "Circular Shape \"Perfect Circle Sculpting Art\"", "円形「真円造形術」"}, {"圆形「圆形造物」", "Circular Shape \"Circle Creature\"", "円形「サークルクリーチャー」"}, {"圆形「圆形造物」", "Circular Shape \"Circle Creature\"", "円形「サークルクリーチャー」"}, {"", "", ""}}},
    {TH17_ST6_BOSS5, 6, 2, 5, 1, false, false, true, {
        {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}, {"", "", ""}}},
    {TH17_ST6_BOSS6, 6, 2, 6, 1, true, false, true, {
        {"线形「线形造形术」", "Linear Shape \"Line-Shaped Sculpting Art\"", "線形「線形造形術」"}, {"线形「线形造形术」", "Linear Shape \"Line-Shaped Sculpting Art\"", "線形「線形造形術」"}, {"线形「线形造物」", "Linear Shape \"Linear Creature\"", "線形「リニアクリーチャー」"}, {"线形「线形造物」", "Linear Shape \"Linear Creature\"", "線形「リニアクリーチャー」"}, {"", "", ""}}},
    {TH17_ST6_BOSS7, 6, 2, 7, 1, false, false, true, {
        {"关底四非", "Boss Nonspell 4", "ボス 通常4"}, {"关底四非", "Boss Nonspell 4", "ボス 通常4"}, {"关底四非", "Boss Nonspell 4", "ボス 通常4"}, {"关底四非", "Boss Nonspell 4", "ボス 通常4"}, {"", "", ""}}},
    {TH17_ST6_BOSS8, 6, 2, 8, 1, true, false, true, {
        {"埴轮「偶像人马造形术」", "Haniwa \"Horse-and-Rider Idol Sculpting Art\"", "埴輪「偶像人馬造形術」"}, {"埴轮「偶像人马造形术」", "Haniwa \"Horse-and-Rider Idol Sculpting Art\"", "埴輪「偶像人馬造形術」"}, {"埴轮「偶像造物」", "Haniwa \"Idol Creature\"", "埴輪「アイドルクリーチャー」"}, {"埴轮「偶像造物」", "Haniwa \"Idol Creature\"", "埴輪「アイドルクリーチャー」"}, {"", "", ""}}},
    {TH17_ST6_BOSS9, 6, 2, 9, 1, true, false, true, {
        {"「鬼形造形术」", "\"Oni-Shaped Sculpting Art\"", "「鬼形造形術」"}, {"「鬼形造形术」", "\"Oni-Shaped Sculpting Art\"", "「鬼形造形術」"}, {"「鬼形造形术」", "\"Oni-Shaped Sculpting Art\"", "「鬼形造形術」"}, {"「鬼形造形术」", "\"Oni-Shaped Sculpting Art\"", "「鬼形造形術」"}, {"", "", ""}}},
    {TH17_ST6_BOSS10, 6, 2, 10, 1, true, true, true, {
        {"「几何造物」", "\"Geometric Creature\"", "「ジオメトリッククリーチャー」"}, {"「几何造物」", "\"Geometric Creature\"", "「ジオメトリッククリーチャー」"}, {"「几何造物」", "\"Geometric Creature\"", "「ジオメトリッククリーチャー」"}, {"「几何造物」", "\"Geometric Creature\"", "「ジオメトリッククリーチャー」"}, {"", "", ""}}},
    {TH17_ST6_BOSS11, 6, 2, 11, 1, true, false, true, {
        {"「偶像恶魔」", "\"Idola Diabolus\"", "「イドラディアボルス」"}, {"「偶像恶魔」", "\"Idola Diabolus\"", "「イドラディアボルス」"}, {"「偶像恶魔」", "\"Idola Diabolus\"", "「イドラディアボルス」"}, {"「偶像恶魔」", "\"Idola Diabolus\"", "「イドラディアボルス」"}, {"", "", ""}}},
    {TH17_ST7_MID1, 7, 1, 1, 0, true, true, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"血战「血之分水岭」", "Bloody Battle \"Watershed of Blood\"", "血戦「血の分水嶺」"}}},
    {TH17_ST7_MID2, 7, 1, 2, 0, true, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"血战「狱界视线」", "Bloody Battle \"Hell Realm Sight-Line\"", "血戦「獄界視線」"}}},
    {TH17_ST7_MID3, 7, 1, 3, 0, true, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"血战「全灵鬼渡」", "Bloody Battle \"All-Spirit Oniwatari\"", "血戦「全霊鬼渡り」"}}},
    {TH17_ST7_END_NS1, 7, 2, 1, 1, false, true, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底一非", "Boss Nonspell 1", "ボス 通常1"}}},
    {TH17_ST7_END_S1, 7, 2, 2, 1, true, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「惊险射击」", "Agile Skill \"Thrilling Shot\"", "勁疾技「スリリングショット」"}}},
    {TH17_ST7_END_NS2, 7, 2, 3, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底二非", "Boss Nonspell 2", "ボス 通常2"}}},
    {TH17_ST7_END_S2, 7, 2, 4, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「闪电爪」", "Agile Skill \"Lightning Neigh\"", "勁疾技「ライトニングネイ」"}}},
    {TH17_ST7_END_NS3, 7, 2, 5, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底三非", "Boss Nonspell 3", "ボス 通常3"}}},
    {TH17_ST7_END_S3, 7, 2, 6, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「浓云」", "Agile Skill \"Dense Crowd\"", "勁疾技「デンスクラウド」"}}},
    {TH17_ST7_END_NS4, 7, 2, 7, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底四非", "Boss Nonspell 4", "ボス 通常4"}}},
    {TH17_ST7_END_S4, 7, 2, 8, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「兽性感染」", "Agile Skill \"Beast Epidemicity\"", "勁疾技「ビーストエピデミシティ」"}}},
    {TH17_ST7_END_NS5, 7, 2, 9, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底五非", "Boss Nonspell 5", "ボス 通常5"}}},
    {TH17_ST7_END_S5, 7, 2, 10, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「三角追击」", "Agile Skill \"Triangle Chase\"", "勁疾技「トライアングルチェイス」"}}},
    {TH17_ST7_END_NS6, 7, 2, 11, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底六非", "Boss Nonspell 6", "ボス 通常6"}}},
    {TH17_ST7_END_S6, 7, 2, 12, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「黑色天马流星弹」", "Agile Skill \"Black Pegasus Meteor Shot\"", "勁疾技「ブラックペガサス流星弾」"}}},
    {TH17_ST7_END_NS7, 7, 2, 13, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底七非", "Boss Nonspell 7", "ボス 通常7"}}},
    {TH17_ST7_END_S7, 7, 2, 14, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"劲疾技「肌肉爆破」", "Agile Skill \"Muscle Explosion\"", "勁疾技「マッスルエクスプロージョン」"}}},
    {TH17_ST7_END_NS8, 7, 2, 15, 1, false, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"关底八非", "Boss Nonspell 8", "ボス 通常8"}}},
    {TH17_ST7_END_S8, 7, 2, 16, 1, true, false, false, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"「Follow Me Unafraid」", "\"Follow Me, Unafraid\"", "「フォロミーアンアフライド」"}}},
    {TH17_ST7_END_S9, 7, 2, 17, 1, true, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"「鬼形的乌合之众」", "\"Oni-Shaped Hoi Polloi\"", "「鬼形のホイポロイ」"}}},
    {TH17_ST7_END_S10, 7, 2, 18, 1, true, false, true, {
        {"", "", ""}, {"", "", ""}, {"", "", ""}, {"", "", ""}, {"「鬼畜生的所业」", "\"Deeds of Devilish Beasts\"", "「鬼畜生の所業」"}}},
};
inline constexpr const char* practice_version = "2.2.2.7";
inline constexpr const char* practice_TH17_GOAST_SELECT[][3] = {
    {"无", "None", "なし"},
    {"狼", "Wolf", "オオカミ"},
    {"獭", "Otter", "カワウソ"},
    {"鹰", "Eagle", "オオワシ"},
    {"Bomb碎片", "Bomb Frag.", "ボムの欠片"},
    {"残机碎片", "Life Frag.", "残機の欠片"},
    {"火力", "Power", "霊力"},
    {"蓝点", "Point", "得点"},
    {"水母", "Jellyfish", "クラゲ"},
    {"牛", "Cow", "牛"},
    {"小鸡", "Chick", "ひよこ"},
    {"龟", "Turtle", "亀"},
    {"埴轮", "Haniwa", "埴輪"},
    {"埴轮（马）", "Haniwa (Horse)", "埴輪（馬）"},
    {"三只小鸡", "3 Chicks", "三匹のひよこ"},
};
inline constexpr const char* practice_TH17_GOAST_SELECT_TOKENS[][3] = {
    {"无", "None", "なし"},
    {"狼 (固定)", "Wolf (static)", "オオカミ (色変わりしない)"},
    {"獭 (固定)", "Otter (static)", "カワウソ (色変わりしない)"},
    {"鹰 (固定)", "Eagle (static)", "オオワシ (色変わりしない)"},
    {"Bomb碎片", "Bomb Frag.", "ボムの欠片"},
    {"残机碎片", "Life Frag.", "残機の欠片"},
    {"火力", "Power", "霊力"},
    {"蓝点", "Point", "得点"},
    {"水母", "Jellyfish", "クラゲ"},
    {"牛", "Cow", "牛"},
    {"小鸡", "Chick", "ひよこ"},
    {"龟", "Turtle", "亀"},
    {"埴轮", "Haniwa", "埴輪"},
    {"埴轮（马）", "Haniwa (Horse)", "埴輪（馬）"},
    {"三只小鸡", "3 Chicks", "三匹のひよこ"},
    {"狼", "Wolf", "オオカミ"},
    {"獭", "Otter", "カワウソ"},
    {"鹰", "Eagle", "オオワシ"},
};
inline constexpr const char* practice_TH_SPELL_PHASE2[][3] = {
    {"P1", "P1", "P1"},
    {"P2", "P2", "P2"},
    {"P3", "P3", "P3"},
    {"P4", "P4", "P4"},
};
inline constexpr const char* practice_TH17_SPELL_5PHASE[][3] = {
    {"P1", "P1", "P1"},
    {"P2", "P2", "P2"},
    {"P3", "P3", "P3"},
    {"P4", "P4", "P4"},
    {"发狂", "Rage", "発狂"},
};
inline constexpr const char* practice_TH_WARP_SELECT[][3] = {
    {"无", "None", "なし"},
    {"道中", "Stage Portion", "道中"},
    {"道中Boss", "Mid Boss", "道中ボス"},
    {"关底Boss", "End Boss", "ボス"},
    {"非符", "Non Spell", "通常"},
    {"符卡", "Spell Card", "スペカ"},
};
inline constexpr const char* practice_IGI_DIFF[][3] = {
    {"Easy", "Easy", "Easy"},
    {"Normal", "Normal", "Normal"},
    {"Hard", "Hard", "Hard"},
    {"Lunatic", "Lunatic", "Lunatic"},
    {"Extra", "Extra", "Extra"},
    {"Phantasm", "Phantasm", "Phantasm"},
    {"Overdrive", "Overdrive", "Overdrive"},
    {"Lastword", "Lastword", "Lastword"},
};
}
