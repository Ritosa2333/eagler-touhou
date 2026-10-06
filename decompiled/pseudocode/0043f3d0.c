// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f3d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f3d0_1 {
    char padding_0[12];
    struct st_43f3d0_2 *field_c;
    struct st_43f3d0_0 *field_10;
    unsigned int field_14;
    unsigned int field_18;
} st_43f3d0_1;

typedef struct st_43f3d0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43f3d0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43f3d0_1 *field_20;
} st_43f3d0_0;

typedef struct st_43f3d0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43f3d0_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43f3d0_1 *field_20;
} st_43f3d0_2;

extern int g_49f434;
extern st_43f3d0_1 *g_4b4530;
extern unsigned int g_4ce55c;

unsigned int sub_43f3d0(void)
{
    st_43f3d0_2 *idx;  // eax
    unsigned int v6;  // eax
    st_43f3d0_2 *index;  // eax
    st_43f3d0_2 *idx1;  // esi
    unsigned int v9;  // ecx
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    idx = sub_46c9ea(36, v0, v1, v2, v3);
    if (idx)
    {
        idx->field_4 = idx->field_4 & 0xfffffffe;
        idx->field_8 = 0;
        idx->field_c = 0;
        idx->field_10 = 0;
        idx->field_0 = 0;
        idx->field_14 = idx;
        idx->field_18 = 0;
        idx->field_1c = 0;
    }
    else
    {
        idx = NULL;
    }
    v6 = idx->field_4 & 0xfffffffd;
    idx->field_8 = sub_43fc90;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_20 = g_4b4530;
    idx->field_4 = v6 | 1;
    sub_462380();
    g_4b4530->field_c = idx;
    index = sub_46c9ea(36);
    if (index)
    {
        index->field_4 = index->field_4 & 0xfffffffe;
        index->field_8 = 0;
        index->field_c = 0;
        index->field_10 = 0;
        index->field_0 = 0;
        index->field_14 = index;
        index->field_18 = 0;
        index->field_1c = 0;
        idx1 = index;
    }
    else
    {
        idx1 = NULL;
    }
    v9 = idx1->field_4 & 0xfffffffd;
    idx1->field_8 = sub_43fca0;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_20 = g_4b4530;
    idx1->field_4 = v9 | 1;
    sub_462420();
    g_4b4530->field_10 = idx1;
    v10 = sub_45fe60();
    g_4b4530->field_14 = v10;
    if (v10)
    {
        v11 = sub_45fe60();
        g_4b4530->field_18 = v11;
        if (v11)
        {
            g_4b4530[8].field_18 = 1;
            g_4ce55c = 0;
            return 0;
        }
    }
    sub_464220(&g_49f434);
    return 0xffffffff;
}
