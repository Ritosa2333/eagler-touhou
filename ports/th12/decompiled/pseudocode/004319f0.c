// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4319f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4319f0_3 {
    char padding_0[4];
    struct st_4319f0_2 *field_4;
    char padding_8[12];
    int field_14;
    char padding_18[4];
    unsigned int field_1c;
} st_4319f0_3;

typedef struct st_4319f0_2 {
    struct st_4319f0_0 *field_0;
} st_4319f0_2;

typedef struct st_4319f0_1 {
    unsigned int field_0;
} st_4319f0_1;

typedef struct st_4319f0_0 {
    char padding_0[72];
    struct st_4319f0_1 *field_48;
} st_4319f0_0;

extern st_4319f0_3 *g_4d4754;

void sub_4319f0(void)
{
    st_4319f0_3 *idx;  // ecx
    unsigned int v3;  // edx
    unsigned int v4;  // esi
    unsigned int v5;  // eax
    st_4319f0_2 **v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    unsigned int v0;  // [bp-0x4]

    idx = g_4d4754;
    if (!g_4d4754)
        return;
    v3 = &g_4d4754->field_1c;
    v0 = v4;
    if (g_4d4754->field_1c == 1)
    {
        v5 = g_4d4754->field_14 - 1;
        g_4d4754->field_14 = v5;
        if (v5 <= NULL)
        {
            v6 = g_4d4754->field_4;
            *((unsigned int *)v3) = 0;
            *(v6)->field_0->field_48(*(v6));
        }
        else
        {
            sub_4663e0();
        }
        idx = g_4d4754;
    }
    if (idx->field_1c == 2)
    {
        v7 = idx->field_14 - 1;
        idx->field_14 = v7;
        if (v7 <= NULL)
            idx->field_1c = 0;
        else
            sub_4663e0();
        idx = g_4d4754;
    }
    if (idx->field_1c == 4)
    {
        v8 = idx->field_14 - 1;
        idx->field_14 = v8;
        if (v8 <= NULL)
            idx->field_1c = 0;
        else
            sub_4663e0();
        idx = g_4d4754;
    }
    if (idx->field_1c != 3)
        return;
    v9 = idx->field_14 - 1;
    idx->field_14 = v9;
    if (v9 > NULL)
    {
        sub_4663e0();
        return;
    }
    idx->field_1c = 0;
    return;
}
