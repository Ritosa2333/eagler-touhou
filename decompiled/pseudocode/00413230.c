// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x413230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_413230_0 {
    unsigned int field_0;
    struct st_413230_0 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[4096];
    unsigned int field_1010;
    unsigned int field_1014;
    unsigned int field_1018;
    struct st_413230_0 *field_101c;
    unsigned int field_1020;
    char padding_1024[4];
    unsigned int field_1028;
    unsigned int field_102c;
    struct st_413230_0 *field_1030;
    unsigned int field_1034;
    unsigned int field_1038;
    unsigned int field_103c;
    char padding_1040[208];
    unsigned int field_1110;
    unsigned int field_1114;
    unsigned int field_1118;
    unsigned int field_111c;
    char padding_1120[312];
    unsigned int field_1258;
    unsigned int field_125c;
    char padding_1260[24];
    unsigned int field_1278;
    char padding_127c[48];
    unsigned int field_12ac;
    unsigned int field_12b0;
    unsigned int field_12b4;
    char padding_12b8[4];
    unsigned int field_12bc;
    struct st_413230_0 *field_12c0;
    unsigned int field_12c4;
    unsigned int field_12c8;
    char padding_12cc[68];
    unsigned int field_1310;
    char padding_1314[72];
    unsigned int field_135c;
    char padding_1360[56];
    unsigned int field_1398;
    char padding_139c[56];
    unsigned int field_13d4;
    char padding_13d8[56];
    unsigned int field_1410;
    char padding_1414[56];
    unsigned int field_144c;
    char padding_1450[56];
    unsigned int field_1488;
    char padding_148c[56];
    unsigned int field_14c4;
    char padding_14c8[4480];
    unsigned int field_2648;
    unsigned int field_264c;
    unsigned int field_2650;
    char padding_2654[8];
    unsigned int field_265c;
    unsigned int field_2660;
    char padding_2664[76];
    unsigned int field_26b0;
    unsigned int field_26b4;
    char padding_26b8[20];
    unsigned int field_26cc;
    unsigned int field_26d0;
    unsigned int field_26d4;
    unsigned int field_26d8;
    char padding_26dc[4];
    unsigned int field_26e0;
    unsigned int field_26e4;
    unsigned int field_26e8;
    unsigned int field_26ec;
    char padding_26f0[4];
    unsigned int field_26f4;
    char padding_26f8[12];
    unsigned int field_2704;
    char padding_2708[4];
    unsigned int field_270c;
    unsigned int field_2710;
    unsigned int field_2714;
    char padding_2718[4];
    unsigned int field_271c;
    unsigned int field_2720;
    unsigned int field_2724;
    char padding_2728[4];
    unsigned int field_272c;
    unsigned int field_2730;
    unsigned int field_2734;
    char padding_2738[4];
    unsigned int field_273c;
    unsigned int field_2740;
    unsigned int field_2744;
    char padding_2748[4];
    unsigned int field_274c;
    unsigned int field_2750;
    unsigned int field_2754;
    char padding_2758[4];
    unsigned int field_275c;
    unsigned int field_2760;
    unsigned int field_2764;
    char padding_2768[4];
    unsigned int field_276c;
    unsigned int field_2770;
    unsigned int field_2774;
    char padding_2778[4];
    unsigned int field_277c;
    unsigned int field_2780;
    unsigned int field_2784;
    char padding_2788[4];
    struct st_413230_0 *field_278c;
} st_413230_0;

typedef struct st_413230_1 {
    char padding_0[100];
    unsigned int field_64;
} st_413230_1;

extern char g_49fc50;
extern char g_4b2ed0;
extern st_413230_1 *g_4b43dc;

st_413230_0 * sub_413230(unsigned int a0)
{
    st_413230_0 *idx;  // edi
    int v5;  // esi
    int v6;  // ebp
    int v7;  // ebx
    st_413230_0 *v8;  // eax
    st_413230_0 *index;  // ebp
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    st_413230_0 *v13;  // eax
    st_413230_0 *v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x10]

    idx->field_1010 = 0;
    idx->field_1014 = 0;
    idx->field_0 = &g_49fc50;
    sub_4123c0(v5, v6, v7);
    sub_477420(idx->padding_1040, 0, 6004);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_1028 = idx->field_1028 & 0xfffffffe;
    idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v8 = &idx->field_8;
    idx->field_4 = v8;
    idx->field_c = 0;
    idx->field_101c = idx;
    idx->field_1018 = 0xffffffff;
    idx->field_1020 = 0;
    idx->field_1030 = v8;
    idx->field_1034 = 0;
    idx->field_1038 = 0;
    idx->field_278c = idx;
    idx->field_1310 = 0;
    idx->field_135c = 0;
    idx->field_1398 = 0;
    idx->field_13d4 = 0;
    idx->field_1410 = 0;
    idx->field_144c = 0;
    idx->field_1488 = 0;
    idx->field_14c4 = 0;
    idx->field_26cc = 0xffffffff;
    sub_477420(&idx->padding_1040[156], 0, 52);
    sub_477420(&idx->padding_1040[104], 0, 52);
    sub_477420(&idx->padding_1040[52], 0, 52);
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        idx->field_1110 = /* unsupported instruction */;
    else
        idx->field_1110 = nan;
    v2 = 88;
    if (/* unsupported instruction */)
        idx->field_1114 = /* unsupported instruction */;
    else
        idx->field_1114 = nan;
    idx->field_2704 = 0xffffffff;
    if (/* unsupported instruction */)
        idx->field_1118 = /* unsupported instruction */;
    else
        idx->field_1118 = nan;
    index = &idx->field_2660;
    if (/* unsupported instruction */)
    {
        idx->field_111c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_111c = nan;
        /* unsupported instruction */
    }
    v1 = 0;
    v0 = index;
    idx->field_12c0 = idx;
    idx->field_12c4 = 0;
    idx->field_12c8 = 0;
    sub_477420();
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        *((int *)&index->padding_10[0x44]) = /* unsupported instruction */;
    else
        *((unsigned int *)&index->padding_10[0x44]) = nan;
    index->field_0 = 0;
    if (/* unsupported instruction */)
    {
        *((int *)&index->padding_10[64]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&index->padding_10[64]) = nan;
        /* unsupported instruction */
    }
    v10 = idx->field_12bc;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_12bc & 1))
    {
        if (/* unsupported instruction */)
            idx->field_12b4 = /* unsupported instruction */;
        else
            idx->field_12b4 = nan;
        idx->field_12b0 = 0;
        idx->field_12ac = 0xfff0bdc1;
        *((char **)&idx->padding_12b8[0]) = &g_4b2ed0;
        idx->field_12bc = v10 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_12b4 = /* unsupported instruction */;
    else
        idx->field_12b4 = nan;
    idx->field_12b0 = 0;
    idx->field_12ac = 0xffffffff;
    v11 = idx->field_26e0;
    if (!((char)idx->field_26e0 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_26d8 = /* unsupported instruction */;
        else
            idx->field_26d8 = nan;
        idx->field_26d4 = 0;
        idx->field_26d0 = 0xfff0bdc1;
        *((char **)&idx->padding_26dc[0]) = &g_4b2ed0;
        idx->field_26e0 = v11 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_26d8 = /* unsupported instruction */;
    else
        idx->field_26d8 = nan;
    idx->field_26d4 = 0;
    idx->field_26d0 = 0xffffffff;
    v12 = idx->field_26f4;
    if (!((char)idx->field_26f4 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_26ec = /* unsupported instruction */;
        else
            idx->field_26ec = nan;
        idx->field_26e8 = 0;
        idx->field_26e4 = 0xfff0bdc1;
        *((char **)&idx->padding_26f0[0]) = &g_4b2ed0;
        idx->field_26f4 = v12 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_26ec = /* unsupported instruction */;
    else
        idx->field_26ec = nan;
    idx->field_26e8 = 0;
    idx->field_26e4 = 0xffffffff;
    idx->field_1278 = 1;
    idx->field_103c = 0;
    v3 = a0;
    idx->field_102c = g_4b43dc->field_64;
    v13 = sub_4694f0();
    idx->field_4->field_4 = v13;
    idx->field_4->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_265c = idx->field_265c & 0xfffffffd;
    idx->field_2648 = 0;
    idx->field_264c = 0;
    idx->field_2650 = 0;
    idx->field_270c = 0xffffffff;
    idx->field_2710 = 0xffffffff;
    idx->field_2714 = 0;
    idx->field_271c = 0xffffffff;
    idx->field_2720 = 0xffffffff;
    idx->field_2724 = 0;
    idx->field_272c = 0xffffffff;
    idx->field_2730 = 0xffffffff;
    idx->field_2734 = 0;
    idx->field_273c = 0xffffffff;
    idx->field_2740 = 0xffffffff;
    idx->field_2744 = 0;
    idx->field_274c = 0xffffffff;
    idx->field_2750 = 0xffffffff;
    idx->field_2754 = 0;
    idx->field_275c = 0xffffffff;
    idx->field_2760 = 0xffffffff;
    idx->field_2764 = 0;
    idx->field_276c = 0xffffffff;
    idx->field_2770 = 0xffffffff;
    idx->field_2774 = 0;
    idx->field_277c = 0xffffffff;
    idx->field_2780 = 0xffffffff;
    idx->field_2784 = 0;
    memset(&idx->padding_1120[0x100], -0x1, 56);
    idx->field_1258 = 0xffffffff;
    idx->field_125c = 0xffffffff;
    return idx;
}
