// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424bd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_424bd0_0 {
    unsigned int field_0;
    struct st_424bd0_0 *field_4;
    struct st_424bd0_0 *field_8;
    struct st_424bd0_0 *field_c;
    struct st_424bd0_0 *field_10;
    struct st_424bd0_0 *field_14;
    char padding_18[72];
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[8];
    unsigned int field_7c;
    char padding_80[4];
    unsigned int field_84;
} st_424bd0_0;

typedef struct st_424bd0_2 {
    unsigned int field_0;
    struct st_424bd0_0 *field_4;
    struct st_424bd0_0 *field_8;
} st_424bd0_2;

extern unsigned int g_4b0cb0;
extern char g_4b0cb8;
extern unsigned int g_4b0cbc;
extern unsigned int g_4b0cc0;

st_424bd0_0 * sub_424bd0(unsigned int a0, int a1)
{
    st_424bd0_0 *idx;  // esi
    unsigned int v2;  // eax
    st_424bd0_0 *iter;  // eax
    st_424bd0_0 *idx1;  // ecx
    st_424bd0_2 *idx2;  // edi
    unsigned int v6;  // eax
    int v0;  // [bp-0x4]

    idx = sub_46c9ea(0x88, v0);
    if (idx)
    {
        sub_477420(idx, 0, 0x88);
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_c = idx;
        idx->field_10 = NULL;
        idx->field_14 = NULL;
        idx->field_7c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_84 = 0xffffffff;
        idx->field_6c = 300;
        idx->field_70 = 0xffffffff;
    }
    else
    {
        idx = NULL;
    }
    v2 = g_4b0cb0 * 3 + 30;
    iter = a0 + v2 * 4;
    idx1 = &idx->field_c;
    if (*((int *)(a0 + v2 * 4 + 4)))
    {
        do
        {
            iter = iter->field_4;
        } while (iter->field_4);
    }
    if (iter->field_4)
    {
        idx1->field_4 = iter->field_4;
        iter->field_4->field_8 = idx1;
    }
    iter->field_4 = idx1;
    idx1->field_8 = iter;
    idx->field_70 = 30;
    idx->field_0 = idx2->field_0;
    idx->field_4 = idx2->field_4;
    idx->field_8 = idx2->field_8;
    sub_46d290(&idx->padding_18[4], a1, 64);
    idx->field_64 = 0;
    idx->field_68 = *((int *)&g_4b0cb8);
    v6 = g_4b0cc0;
    if (!*((int *)&g_4b0cb8))
        v6 = g_4b0cbc;
    idx->field_60 = v6;
    return idx;
}
