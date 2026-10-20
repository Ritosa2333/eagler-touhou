// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439080; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_439080_1 {
    char padding_0[84];
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[16];
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[60];
    int field_b0;
    char padding_b4[24];
    unsigned int field_cc;
} st_439080_1;

typedef struct st_439080_2 {
    char padding_0[92];
    unsigned int field_5c;
    unsigned int field_60;
    char padding_64[8];
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[60];
    int field_b0;
    char padding_b4[24];
    unsigned int field_cc;
} st_439080_2;

typedef struct st_439080_0 {
    char padding_0[50584];
    unsigned int field_c598;
} st_439080_0;

extern st_439080_0 *g_4b4514;

void sub_439080(void)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // edi
    unsigned int v5;  // edi
    st_439080_2 *index;  // esi
    st_439080_1 *idx;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    v5 = g_4b4514->field_c598;
    if (!v5)
    {
        if (index->field_cc)
            sub_461970(index->field_b0);
        index->field_6c = index->field_5c;
        index->field_70 = index->field_60;
        index->field_cc = 0;
        return;
    }
    else
    {
        if (!idx->field_cc)
            sub_461970(idx->field_b0);
        idx->field_54 = idx->field_6c;
        idx->field_58 = idx->field_70;
        idx->field_cc = v5;
        return;
    }
}
