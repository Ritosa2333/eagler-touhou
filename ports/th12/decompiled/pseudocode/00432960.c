// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x432960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_432960_1 {
    char padding_0[512];
    unsigned int field_200;
} st_432960_1;

typedef struct st_432960_0 {
    char padding_0[96];
    unsigned int field_60;
} st_432960_0;

extern char g_4b2ed0;
extern st_432960_0 *g_4b44e8;
extern unsigned int g_4cf468;

void sub_432960(void)
{
    int v1;  // edi
    st_432960_1 *v2;  // esi

    g_4b44e8->field_60 = g_4b44e8->field_60 & 0xffffffef;
    sub_435670(v1);
    sub_454960(7, 0);
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
    if (/* unsupported instruction */)
    {
        *((int *)&g_4b2ed0) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&g_4b2ed0) = nan;
        /* unsupported instruction */
    }
    g_4cf468 = v2->field_200;
    return;
}
