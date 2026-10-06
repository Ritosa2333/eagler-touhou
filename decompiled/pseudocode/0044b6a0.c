// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b6a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b6a0_0 {
    char padding_0[8];
    unsigned int field_8;
    struct st_44b6a0_1 *field_c;
} st_44b6a0_0;

typedef struct st_44b6a0_1 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_44b6a0_1;

extern char g_4a241c;

int sub_44b6a0(void)
{
    unsigned int *idx;  // eax
    st_44b6a0_0 *index;  // ecx
    unsigned int v5;  // eax
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    sub_44b710(v0, v1);
    idx = sub_46c9ea(16);
    if (idx)
    {
        idx[1] = 0;
        idx[2] = 0;
        idx[3] = 0;
        *(idx) = &g_4a241c;
    }
    else
    {
        idx = NULL;
    }
    index->field_c = idx;
    if (!idx)
        return;
    if ((char)sub_44b960())
    {
        v5 = sub_44bc20();
        index->field_8 = v5;
        if (v5)
            return;
    }
    sub_44b710();
    return;
}
