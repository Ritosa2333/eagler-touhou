// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421350_0 {
    char padding_0[27820];
    unsigned int field_6cac;
    char padding_6cb0[104];
    unsigned int field_6d18;
    unsigned int field_6d1c;
    unsigned int field_6d20;
    unsigned int field_6d24;
    char padding_6d28[4];
    unsigned int field_6d2c;
    char padding_6d30[20];
    int field_6d44;
    unsigned int field_6d48;
} st_421350_0;

extern int g_4b0c44;
extern unsigned int g_4b0cb0;
extern char g_4b2ed0;
extern st_421350_0 *g_4b43e4;

void sub_421350(void)
{
    int v2;  // esi
    unsigned int v3;  // ecx
    unsigned int v4;  // eax
    char v0;  // [bp-0x4]

    sub_4615a0(g_4b43e4->field_6d44, &v0, 0x55, 0, v2);
    g_4b43e4->field_6cac = 0x55;
    v3 = g_4b0cb0 * 1000000;
    g_4b43e4->field_6d48 = v3;
    g_4b0c44 = g_4b0c44 + ((unsigned int)((int)(1717986919 * v3 >> 0x22)) >> 31) + (int)(1717986919 * v3 >> 0x22);
    if (g_4b0c44 >= 1000000000)
        g_4b0c44 = 0x3b9ac9ff;
    g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 0x200;
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = g_4b43e4->field_6d2c;
    if (!((char)g_4b43e4->field_6d2c & 1))
    {
        if (/* unsupported instruction */)
            g_4b43e4->field_6d24 = /* unsupported instruction */;
        else
            g_4b43e4->field_6d24 = nan;
        g_4b43e4->field_6d20 = 0;
        g_4b43e4->field_6d1c = 0xfff0bdc1;
        *((char **)&g_4b43e4->padding_6d28[0]) = &g_4b2ed0;
        g_4b43e4->field_6d2c = v4 | 1;
    }
    if (/* unsupported instruction */)
    {
        g_4b43e4->field_6d24 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43e4->field_6d24 = nan;
        /* unsupported instruction */
    }
    g_4b43e4->field_6d20 = 0;
    g_4b43e4->field_6d1c = 0xffffffff;
    return;
}
