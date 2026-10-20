// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ccd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ccd0_4 {
    char padding_0[6312];
    struct st_43ccd0_1 *field_18a8;
    struct st_43ccd0_0 *field_18ac;
} st_43ccd0_4;

typedef struct st_43ccd0_0 {
    char padding_0[4];
    struct st_43ccd0_1 *field_4;
} st_43ccd0_0;

typedef struct st_43ccd0_12 {
    struct st_43ccd0_4 *field_0;
    struct st_43ccd0_12 *field_4;
} st_43ccd0_12;

typedef struct st_43ccd0_1 {
    char padding_0[8];
    struct st_43ccd0_0 *field_8;
} st_43ccd0_1;

int sub_43ccd0(void)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // eax
    st_43ccd0_12 *v4;  // eax
    unsigned int v5;  // esi
    st_43ccd0_12 *v6;  // esi
    st_43ccd0_4 *idx;  // eax
    unsigned int v0;  // [bp-0x8]

    v4 = *((int *)(v2 + v3 * 12 + 0x44));
    if (!v4)
        return;
    v0 = v5;
    do
    {
        v6 = v4->field_4;
        idx = v4->field_0;
        if (idx)
        {
            if (idx->field_18a8)
                idx->field_18a8->field_8 = idx->field_18ac;
            if (idx->field_18ac)
                idx->field_18ac->field_4 = idx->field_18a8;
            idx->field_18a8 = NULL;
            idx->field_18ac = NULL;
            sub_46ca4f(idx);
        }
    } while ((v4 = v6, v4));
    return;
}
