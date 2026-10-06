// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4627e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4627e0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_4627e0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
} st_4627e0_0;

extern unsigned int g_4;
extern unsigned int g_8;
extern unsigned int g_c;
extern unsigned int g_10;

st_4627e0_0 * sub_4627e0(unsigned int a0)
{
    st_4627e0_0 *idx;  // eax

    idx = sub_46c9ea(36);
    if (!idx)
    {
        g_4 = g_4 | 1;
        g_8 = a0;
        g_c = 0;
        g_10 = 0;
        return NULL;
    }
    idx->field_4 = idx->field_4 & 0xfffffffe;
    idx->field_8 = 0;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_0 = 0;
    idx->field_14 = idx;
    idx->field_18 = 0;
    idx->field_1c = 0;
    idx->field_4 = idx->field_4 | 1;
    idx->field_8 = a0;
    idx->field_c = 0;
    idx->field_10 = 0;
    return idx;
}
