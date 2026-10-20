// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421430; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421430_0 {
    char padding_0[27820];
    int field_6cac;
    int field_6cb0;
    char padding_6cb4[100];
    unsigned int field_6d18;
    unsigned int field_6d1c;
    unsigned int field_6d20;
    unsigned int field_6d24;
    char padding_6d28[4];
    unsigned int field_6d2c;
} st_421430_0;

extern char g_4b2ed0;
extern st_421430_0 *g_4b43e4;

void sub_421430(void)
{
    int v1;  // esi
    int v2;  // ebx
    unsigned int v3;  // eax

    sub_461970(g_4b43e4->field_6cac, v1, v2);
    sub_461970(g_4b43e4->field_6cb0);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b43e4->field_6d18 = g_4b43e4->field_6d18 & 0xfffffdff;
    v3 = g_4b43e4->field_6d2c;
    if (!((char)g_4b43e4->field_6d2c & 1))
    {
        if (/* unsupported instruction */)
            g_4b43e4->field_6d24 = /* unsupported instruction */;
        else
            g_4b43e4->field_6d24 = nan;
        g_4b43e4->field_6d20 = 0;
        g_4b43e4->field_6d1c = 0xfff0bdc1;
        *((char **)&g_4b43e4->padding_6d28[0]) = &g_4b2ed0;
        g_4b43e4->field_6d2c = v3 | 1;
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
