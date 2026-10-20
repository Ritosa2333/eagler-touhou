// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41add0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41add0_1 {
    struct st_41add0_0 *field_0;
    struct st_41add0_1 *field_4;
} st_41add0_1;

typedef struct st_41add0_0 {
    char padding_0[9976];
    unsigned int field_26f8;
} st_41add0_0;

typedef struct st_41add0_2 {
    char padding_0[104];
    struct st_41add0_1 *field_68;
} st_41add0_2;

extern st_41add0_2 *g_4b43dc;

unsigned int sub_41add0(void)
{
    st_41add0_1 *v1;  // eax
    st_41add0_1 *v2;  // edx

    v1 = g_4b43dc->field_68;
    if (!v1)
        return 0;
    do
    {
        v2 = v1->field_4;
        if (v1->field_0->field_26f8 & 0x400)
            v1->field_0->field_26f8 = v1->field_0->field_26f8 & 0xfffffbff | 0x100;
    } while ((v1 = v2, v1));
    return 0;
}
