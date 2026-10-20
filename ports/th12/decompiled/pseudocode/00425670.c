// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425670; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425670_1 {
    struct st_425670_0 *field_0;
    struct st_425670_1 *field_4;
} st_425670_1;

typedef struct st_425670_0 {
    char padding_0[112];
    int field_70;
} st_425670_0;

extern unsigned int g_4b0cb0;

st_425670_1 * sub_425670(unsigned int a0)
{
    st_425670_1 *v1;  // eax
    st_425670_1 *v2;  // edx

    v1 = *((int *)(a0 + g_4b0cb0 * 12 + 124));
    if (!v1)
        return v1;
    do
    {
        v2 = v1->field_4;
        if (v1->field_0->field_70 > NULL)
            v1->field_0->field_70 = v1->field_0->field_70 - 1;
    } while ((v1 = v2, v1));
    return v1;
}
