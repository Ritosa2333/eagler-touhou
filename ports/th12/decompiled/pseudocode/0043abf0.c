// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43abf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43abf0_0 {
    char padding_0[2732];
    unsigned int field_aac;
    char padding_ab0[116];
    char field_b24;
    char padding_b25[32347];
    unsigned int field_8980;
    char field_8984;
} st_43abf0_0;

extern st_43abf0_0 *g_4b4514;

unsigned int sub_43abf0(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // ecx
    unsigned int v4;  // eax

    if (g_4b4514->field_8980 == a1)
    {
        g_4b4514->field_8980 = 0;
        g_4b4514->field_8984 = 0;
    }
    v1 = &g_4b4514->field_aac;
    v2 = 0x100;
    do
    {
        v3 = v2;
        v4 = v1;
        if (*((int *)v4) == a1)
            *((unsigned int *)v4) = 0;
    } while ((v1 = v4 + 120, v2 = v3 - 1, v3 != 1));
    return v4 + 120;
}
