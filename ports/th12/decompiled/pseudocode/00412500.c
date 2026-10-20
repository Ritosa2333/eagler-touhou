// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412500; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412500_0 {
    unsigned int field_0;
    struct st_412500_0 *field_4;
} st_412500_0;

typedef struct st_412500_1 {
    char padding_0[104];
    struct st_412500_0 *field_68;
} st_412500_1;

extern st_412500_1 *g_4b43dc;

unsigned int sub_412500(unsigned int a0)
{
    st_412500_0 *v1;  // eax
    st_412500_0 *v2;  // eax

    if (!a0)
        return 0;
    v1 = g_4b43dc->field_68;
    if (!g_4b43dc->field_68)
        return 0;
    while (1)
    {
        v2 = v1;
        if (*((int *)(v2->field_0 + 10164)) == a0)
            return 1;
        v1 = v2->field_4;
        if (!v2->field_4)
            return 0;
    }
}
