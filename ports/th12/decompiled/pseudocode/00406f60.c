// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406f60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406f60_0 {
    char padding_0[50176];
    unsigned int field_c400;
    unsigned int field_c404;
    unsigned int field_c408;
    char padding_c40c[4];
    unsigned int field_c410;
} st_406f60_0;

extern char g_4b2ed0;
extern st_406f60_0 *g_4b4514;

void sub_406f60(unsigned int a0)
{
    unsigned int v0;  // ecx

    v0 = g_4b4514->field_c410;
    if (!((char)v0 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4514->field_c408 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b4514->field_c404 = 0;
        g_4b4514->field_c400 = 0xfff0bdc1;
        *((char **)&g_4b4514->padding_c40c[0]) = &g_4b2ed0;
        g_4b4514->field_c410 = v0 | 1;
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    g_4b4514->field_c404 = a0;
    g_4b4514->field_c400 = a0 - 1;
    if (!/* unsupported instruction */)
    {
        g_4b4514->field_c408 = nan;
        /* unsupported instruction */
        return;
    }
    g_4b4514->field_c408 = /* unsupported instruction */;
    /* unsupported instruction */
    return;
}
