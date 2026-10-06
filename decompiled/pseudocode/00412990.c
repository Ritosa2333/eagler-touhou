// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412990; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412990_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
} st_412990_1;

typedef struct st_412990_0 {
    char padding_0[4];
    struct st_412990_0 *field_4;
    struct st_412990_0 *field_8;
    char padding_c[4120];
    char field_1024;
    char padding_1025[131];
    unsigned int field_10a8;
    unsigned int field_10ac;
    unsigned int field_10b0;
    char padding_10b4[432];
    unsigned int field_1264;
    unsigned int field_1268;
    char padding_126c[16];
    unsigned int field_127c;
    char padding_1280[68];
    struct st_412990_0 *field_12c4;
    struct st_412990_0 *field_12c8;
    char padding_12cc[4984];
    unsigned int field_2644;
    int field_2648;
    int field_264c;
    char padding_2650[16];
    unsigned int field_2660;
    char padding_2664[84];
    unsigned int field_26b8;
    unsigned int field_26bc;
    unsigned int field_26c0;
    char padding_26c4[20];
    unsigned int field_26d8;
    char padding_26dc[4];
    unsigned int field_26e0;
    char padding_26e4[20];
    unsigned int field_26f8;
    char padding_26fc[184];
    unsigned int field_27b4;
} st_412990_0;

typedef struct st_412990_3 {
    char padding_0[104];
    struct st_412990_0 *field_68;
    struct st_412990_0 *field_6c;
    unsigned int field_70;
    unsigned int field_74;
    unsigned int field_78;
} st_412990_3;

extern unsigned int g_4b0ca8;
extern char g_4b2ed0;
extern st_412990_3 *g_4b43dc;

st_412990_0 * sub_412990(st_412990_1 *index, unsigned int a1)
{
    int v0;  // edi
    int v1;  // esi
    unsigned int v10;  // ecx
    unsigned int v11;  // ecx
    st_412990_0 *idx;  // eax
    st_412990_0 *idx1;  // ecx
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    int v2;  // ebp
    int v3;  // ebx
    int v4;  // ecx
    st_412990_0 *idx2;  // ebp
    st_412990_1 *i;  // esi
    st_412990_0 *iter;  // edi
    unsigned int v9;  // ecx

    idx2 = (!sub_46c9ea(10168, v0, v1, v2, v3, v4) ? NULL : sub_413230(index));
    idx2->field_10a8 = index->field_0;
    idx2->field_10ac = index->field_4;
    idx2->field_10b0 = index->field_8;
    idx2->field_2644 = index->field_c;
    idx2->field_2648 = index->field_14;
    idx2->field_264c = index->field_14;
    idx2->field_2660 = index->field_10;
    idx2->field_26f8 = idx2->field_26f8 ^ (index->field_18 * 0x40000 ^ idx2->field_26f8) & 0x40000;
    i = &index->field_20;
    iter = &idx2->field_127c;
    v9 = 12;
    for (idx2->field_1024 = 1 << ((char)g_4b0ca8 & 31); v9; i = &i->field_4)
    {
        v9 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = i->field_0;
        iter = &iter->field_4;
    }
    v10 = idx2->field_26e0;
    if (!((char)idx2->field_26e0 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        idx2->field_26d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&idx2->padding_26c4[16]) = 0;
        *((unsigned int *)&idx2->padding_26c4[12]) = 0xfff0bdc1;
        *((char **)&idx2->padding_26dc[0]) = &g_4b2ed0;
        idx2->field_26e0 = v10 | 1;
    }
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
    *((unsigned int *)&idx2->padding_26c4[16]) = 2;
    if (/* unsupported instruction */)
    {
        idx2->field_26d8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx2->field_26d8 = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&idx2->padding_26c4[12]) = 1;
    v11 = index->field_1c * 0x2000000 ^ idx2->field_26f8;
    *((unsigned int *)&idx2->padding_126c[8]) = 0;
    idx2->field_26f8 = idx2->field_26f8 ^ v11 & 0x2000000;
    if (index->field_14 >= 1000)
        idx2->field_26f8 = idx2->field_26f8 | 0x20000000;
    sub_413840(&idx2->padding_1025[27]);
    idx2->field_27b4 = g_4b43dc->field_74;
    idx2->field_26b8 = (g_4b43dc->field_74 & 1) + 3;
    idx2->field_26bc = 84;
    if (idx2->field_1264 == 1)
    {
        switch (idx2->field_1268)
        {
        case 0: case 54:
            idx2->field_26bc = 84;
            break;
        case 5: case 55:
            idx2->field_26bc = 81;
            break;
        case 10: case 56:
            idx2->field_26bc = 87;
            break;
        case 15: case 57:
            idx2->field_26bc = 90;
            break;
        }
    }
    idx2->field_26c0 = 0;
    idx = &idx2->padding_1280[64];
    if (!g_4b43dc->field_68)
    {
        g_4b43dc->field_68 = idx;
    }
    else
    {
        idx1 = g_4b43dc->field_6c;
        if (idx1->field_4)
        {
            idx->field_4 = idx1->field_4;
            idx1->field_4->field_8 = idx;
        }
        idx1->field_4 = idx;
        idx->field_8 = idx1;
    }
    g_4b43dc->field_70 = g_4b43dc->field_70 + 1;
    g_4b43dc->field_6c = idx;
    v14 = g_4b43dc->field_74;
    g_4b43dc->field_78 = v14;
    v15 = v14 + 1;
    g_4b43dc->field_74 = v15;
    if (!v15)
        g_4b43dc->field_74 = 1;
    return idx2;
}
