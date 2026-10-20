// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x436100; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_436100_0 {
    char padding_0[2592];
    unsigned int field_a20;
    unsigned int field_a24;
    unsigned int field_a28;
    char padding_a2c[4];
    unsigned int field_a30;
    unsigned int field_a34;
    unsigned int field_a38;
    char padding_a3c[4];
    unsigned int field_a40;
    unsigned int field_a44;
    unsigned int field_a48;
    unsigned int field_a4c;
    char padding_a50[4];
    unsigned int field_a54;
    char padding_a58[30720];
    unsigned int field_8258;
    unsigned int field_825c;
    char padding_8260[16820];
    unsigned int field_c414;
    char padding_c418[8];
    unsigned int field_c420;
    unsigned int field_c424;
    unsigned int field_c428;
    char padding_c42c[4];
    unsigned int field_c430;
} st_436100_0;

extern int g_4b0c98;
extern int g_4b0c9c;
extern char g_4b2ed0;
extern int g_4b43e4;
extern st_436100_0 *g_4b4514;

void sub_436100(void)
{
    unsigned int v5;  // ecx
    unsigned int v6;  // esi
    unsigned int v7;  // eax
    unsigned int v8;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v3 = v5;
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = v6;
    g_4b4514->field_a28 = 1;
    v7 = g_4b4514->field_c430;
    v1 = v8;
    if (!((char)v7 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4514->field_c428 = /* unsupported instruction */;
        else
            g_4b4514->field_c428 = nan;
        g_4b4514->field_c424 = 0;
        g_4b4514->field_c420 = 0xfff0bdc1;
        *((char **)&g_4b4514->padding_c42c[0]) = &g_4b2ed0;
        g_4b4514->field_c430 = v7 | 1;
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
    g_4b4514->field_c424 = 0xffffffff;
    if (/* unsupported instruction */)
    {
        g_4b4514->field_c428 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b4514->field_c428 = nan;
        /* unsupported instruction */
    }
    g_4b4514->field_c420 = 0xfffffffe;
    v9 = g_4b4514->field_a40;
    if (!((char)g_4b4514->field_a40 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4514->field_a38 = /* unsupported instruction */;
        else
            g_4b4514->field_a38 = nan;
        g_4b4514->field_a34 = 0;
        g_4b4514->field_a30 = 0xfff0bdc1;
        *((char **)&g_4b4514->padding_a3c[0]) = &g_4b2ed0;
        g_4b4514->field_a40 = v9 | 1;
    }
    if (/* unsupported instruction */)
        g_4b4514->field_a38 = /* unsupported instruction */;
    else
        g_4b4514->field_a38 = nan;
    g_4b4514->field_a34 = 0;
    g_4b4514->field_a30 = 0xffffffff;
    v10 = g_4b4514->field_a54;
    if (!((char)g_4b4514->field_a54 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4514->field_a4c = /* unsupported instruction */;
        else
            g_4b4514->field_a4c = nan;
        g_4b4514->field_a48 = 0;
        g_4b4514->field_a44 = 0xfff0bdc1;
        *((char **)&g_4b4514->padding_a50[0]) = &g_4b2ed0;
        g_4b4514->field_a54 = v10 | 1;
    }
    if (/* unsupported instruction */)
    {
        g_4b4514->field_a4c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b4514->field_a4c = nan;
        /* unsupported instruction */
    }
    g_4b4514->field_a48 = 0;
    g_4b4514->field_a44 = 0xffffffff;
    g_4b4514->field_c414 = g_4b4514->field_c414 & 0xfffffff8;
    g_4b4514->field_a20 = 0;
    g_4b4514->field_a24 = 0;
    v0 = g_4b4514->field_8258;
    sub_461a70();
    g_4b4514->field_8258 = 0;
    sub_41ce60(g_4b43e4, g_4b0c98, g_4b0c9c);
    sub_435900(g_4b4514);
    sub_4385b0(g_4b4514);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b4514->field_825c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return;
}
