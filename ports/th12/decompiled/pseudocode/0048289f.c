// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48289f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

extern st_47dc29_1 g_4b42f8;

int sub_48289f(void)
{
    unsigned int v6;  // esi
    unsigned int v7;  // edx
    void* idx;  // eax
    unsigned int v0;  // [bp-0x20]
    char v1;  // [bp-0x1c]
    char v2;  // [bp-0x14]
    char v3;  // [bp-0xc]
    int v4;  // [bp+0x4]
    int v5;  // [bp+0x8]

    v0 = v6;
    idx = sub_47dc29(&g_4b42f8.field_0, v7, 8, 0);
    if (idx)
    {
        *((unsigned int *)idx) = 0;
        *((char *)&idx[4]) = 0;
        *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    }
    else
    {
        idx = NULL;
    }
    sub_4827df(v4, idx);
    sub_47e0c2(&v3);
    sub_47ec6e(&v1, 32, &v2, v5);
    sub_47dde0(idx, v7, sub_47e9ae(&v1, 32, &v2, v5));
    return;
}
