// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449f20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_449f20_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_449f20_0;

extern st_449f20_0 *g_4b43dc;

unsigned int sub_449f20(void)
{
    unsigned int v1;  // eax
    unsigned int v2;  // ecx

    v1 = 0;
    v2 = &g_4b43dc->field_1c;
    while (!*((int *)v2))
    {
        v1 += 1;
        v2 += 4;
        if (v1 >= 8)
            return 0;
    }
    return 1;
}
