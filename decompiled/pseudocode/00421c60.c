// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421c60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421c60_0 {
    char padding_0[80];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[4];
    unsigned int field_60;
    char padding_64[4];
    unsigned int field_68;
    unsigned int field_6c;
} st_421c60_0;

extern char g_4b2ed0;
extern st_421c60_0 *g_4b43dc;

unsigned int sub_421c60(void)
{
    unsigned int v1;  // ecx

    /* unsupported instruction */
    /* unsupported instruction */
    v1 = g_4b43dc->field_60;
    if (!((char)g_4b43dc->field_60 & 1))
    {
        if (/* unsupported instruction */)
            g_4b43dc->field_58 = /* unsupported instruction */;
        else
            g_4b43dc->field_58 = nan;
        g_4b43dc->field_54 = 0;
        g_4b43dc->field_50 = 0xfff0bdc1;
        *((char **)&g_4b43dc->padding_5c[0]) = &g_4b2ed0;
        g_4b43dc->field_60 = v1 | 1;
    }
    if (/* unsupported instruction */)
    {
        g_4b43dc->field_58 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43dc->field_58 = nan;
        /* unsupported instruction */
    }
    g_4b43dc->field_54 = 0;
    g_4b43dc->field_50 = 0xffffffff;
    g_4b43dc->field_68 = 0;
    g_4b43dc->field_6c = 0;
    return 0;
}
