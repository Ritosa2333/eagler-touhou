// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b610; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b610_6 {
    char padding_0[8];
    unsigned int field_8;
    struct st_44b610_1 *field_c;
} st_44b610_6;

typedef struct st_44b610_2 {
    struct st_44b610_0 *field_0;
} st_44b610_2;

typedef struct st_44b610_0 {
    unsigned int field_0;
} st_44b610_0;

typedef struct st_44b610_1 {
    struct st_44b610_2 *field_0;
    unsigned int field_4;
    unsigned int field_8;
} st_44b610_1;

extern char g_4a22dc;

int sub_44b610(void)
{
    int v3;  // eax
    st_44b610_1 *idx;  // eax
    st_44b610_6 *index;  // ecx
    unsigned int v6;  // eax
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    sub_44b710(v0, v1);
    sub_44bcd0("info : %s open arcfile\r\n", v3);
    idx = sub_46c9ea(12);
    if (idx)
    {
        idx->field_0 = &g_4a22dc;
        idx->field_4 = 0xffffffff;
        idx->field_8 = 0;
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
        v6 = sub_44bc20();
        index->field_8 = v6;
        if (v6)
        {
            index->field_c->field_0->field_0(v6, "r");
            return;
        }
    }
    sub_44bcd0("info : %s not found\r\n", v3);
    sub_44b710();
    return;
}
