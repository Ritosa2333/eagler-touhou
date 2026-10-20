// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412960_0 {
    char padding_0[8];
    struct st_412960_0 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_412960_0;

typedef struct st_412960_1 {
    char padding_0[24];
    struct st_412960_0 *field_18;
} st_412960_1;

extern st_412960_1 *g_4b44f4;

st_412960_0 * sub_412960(unsigned int a0)
{
    st_412960_0 *v1;  // eax
    st_412960_0 *v2;  // eax

    v1 = g_4b44f4->field_18;
    if (a0 && g_4b44f4->field_18)
    {
        do
        {
            v2 = v1;
            if (v2->field_80 == a0)
                return v2;
        } while ((v1 = (st_412960_0 *)v2->field_8, v2->field_8));
    }
    return NULL;
}
