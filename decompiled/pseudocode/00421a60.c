// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421a60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421a60_3 {
    char padding_0[22652];
    unsigned short field_587c;
} st_421a60_3;

typedef struct st_421a60_1 {
    char padding_0[22652];
    unsigned short field_587c;
    char padding_587e[206];
    struct st_421a60_2 *field_594c;
} st_421a60_1;

typedef struct st_421a60_2 {
    unsigned int field_0;
} st_421a60_2;

typedef struct st_421a60_0 {
    char padding_0[26504];
    unsigned int field_6788;
} st_421a60_0;

extern st_421a60_0 *g_4b43e4;

int sub_421a60(void)
{
    unsigned int v1;  // esi
    st_421a60_1 *v2;  // ebx
    st_421a60_3 *v11;  // esi
    unsigned int v12;  // eax
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    unsigned int v5;  // esi
    unsigned int v6;  // ebx
    unsigned int v7;  // edx
    st_421a60_3 *v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v10;  // esi

    v1 = g_4b43e4->field_6788 + 1;
    v2 = &g_4b43e4->padding_0[1204 * v1 + 21688];
    v3 = v1 + 1;
    if (*((int *)&v2->padding_0[1172]))
        (*((int *)&v2->padding_0[1172]))(v4, v5, v6);
    *((unsigned short *)&v2->padding_0[964]) = 9;
    sub_455630(v2);
    if (v3 >= 4)
        v3 = 1;
    v7 = v3 * 1204;
    v8 = &g_4b43e4->padding_0[v7 + 21688];
    v9 = v3 + 1;
    if (*((int *)&g_4b43e4->padding_0[22860 + v7]))
        (*((int *)&g_4b43e4->padding_0[22860 + v7]))();
    *((unsigned short *)&v8->padding_0[964]) = 7;
    sub_455630(v8);
    if (v9 >= 4)
        v9 = 1;
    v10 = v9 * 1204;
    v11 = &g_4b43e4->padding_0[v10 + 21688];
    if (*((int *)&g_4b43e4->padding_0[22860 + v10]))
        (*((int *)&g_4b43e4->padding_0[22860 + v10]))();
    *((unsigned short *)&v11->padding_0[964]) = 8;
    sub_455630(v11);
    v12 = g_4b43e4->field_6788 + 1;
    g_4b43e4->field_6788 = v12 % 3;
    return v12 / 3;
}
