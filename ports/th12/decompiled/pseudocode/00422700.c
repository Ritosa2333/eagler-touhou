// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422700; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422700_0 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[60];
    unsigned int field_60;
    char padding_64[16];
    unsigned int field_74;
} st_422700_0;

typedef struct st_422700_2 {
    unsigned int field_0;
} st_422700_2;

typedef struct st_422700_1 {
    char padding_0[20];
    struct st_422700_2 *field_14;
} st_422700_1;

typedef struct st_422700_3 {
    struct st_422700_1 *field_0;
} st_422700_3;

extern unsigned int g_4b44e8;
extern st_422700_3 *g_4ce8f0;
extern unsigned int g_4cf468;

st_422700_0 * sub_422700(unsigned int a0)
{
    st_422700_0 *idx;  // edi
    int v2;  // esi
    int v0;  // [bp-0x4]

    idx = sub_46c9ea(120, v0);
    if (idx)
    {
        idx->field_20 = idx->field_20 & 0xfffffffe;
        sub_423250(v2);
        sub_477420(idx, 0, 120);
    }
    else
    {
        idx = NULL;
    }
    g_4cf468 = 0;
    g_4ce8f0->field_0->field_14(g_4ce8f0);
    idx->field_60 = idx->field_60 | 4;
    g_4b44e8 = idx;
    idx->field_74 = a0;
    sub_430500(v0);
    return idx;
}
