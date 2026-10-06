// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47bf78; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47bf78_0 {
    struct st_47bf78_0 *field_0;
    unsigned int field_4;
    struct st_47bf78_0 *field_8;
    unsigned int field_c;
    char padding_10[8];
    unsigned int field_18;
} st_47bf78_0;

extern unsigned int g_4b42f0;

void sub_47bf78(st_47bf78_0 *idx)
{
    st_47bf78_0 *v1;  // eax
    st_47bf78_0 *v2;  // eax
    char v0;  // [bp+0x0]

    g_4b42f0 = g_4b42f0 + 1;
    v1 = sub_4777ce(0x1000, &v0);
    idx->field_8 = v1;
    if (v1)
    {
        idx->field_c = idx->field_c | 8;
        idx->field_18 = 0x1000;
    }
    else
    {
        idx->field_c = idx->field_c | 4;
        idx->field_8 = &idx->padding_10[4];
        idx->field_18 = 2;
    }
    v2 = idx->field_8;
    idx->field_4 = 0;
    idx->field_0 = v2;
    return;
}
