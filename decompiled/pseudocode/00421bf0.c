// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421bf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421bf0_0 {
    char padding_0[16];
    struct st_421bf0_1 *field_10;
} st_421bf0_0;

typedef struct st_421bf0_1 {
    unsigned int field_0;
} st_421bf0_1;

typedef struct st_421bf0_2 {
    char padding_0[8];
    struct st_421bf0_3 *field_8;
} st_421bf0_2;

typedef struct st_421bf0_3 {
    struct st_421bf0_0 *field_0;
    struct st_421bf0_2 *field_4;
    struct st_421bf0_3 *field_8;
} st_421bf0_3;

typedef struct st_421bf0_4 {
    char padding_0[24];
    struct st_421bf0_3 *field_18;
} st_421bf0_4;

extern st_421bf0_4 *g_4b44f4;

void sub_421bf0(void)
{
    st_421bf0_3 *idx;  // esi
    st_421bf0_3 *v2;  // edi
    unsigned int v3;  // edi

    idx = g_4b44f4->field_18;
    if (!idx)
        return;
    do
    {
        v2 = idx->field_8;
        idx->field_0->field_10(v3);
        idx->field_4->field_8 = idx->field_8;
        if (idx->field_8)
            idx->field_8->field_4 = idx->field_4;
    } while ((sub_46ca4f(idx), idx = v2, idx));
    return;
}
