// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x405890; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_405890_0 {
    char padding_0[13756];
    unsigned int field_35bc;
    unsigned int field_35c0;
    unsigned int field_35c4;
    unsigned int field_35c8;
    char padding_35cc[4];
    unsigned int field_35d0;
} st_405890_0;

extern char g_4b2ed0;
extern st_405890_0 *g_4b43c0;

void sub_405890(void)
{
    unsigned int v1;  // ecx

    v1 = g_4b43c0->field_35d0;
    if (!((char)g_4b43c0->field_35d0 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43c0->field_35c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43c0->field_35c4 = 0;
        g_4b43c0->field_35c0 = 0xfff0bdc1;
        *((char **)&g_4b43c0->padding_35cc[0]) = &g_4b2ed0;
        g_4b43c0->field_35d0 = v1 | 1;
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
    g_4b43c0->field_35c4 = 60;
    if (/* unsupported instruction */)
    {
        g_4b43c0->field_35c8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43c0->field_35c8 = nan;
        /* unsupported instruction */
    }
    g_4b43c0->field_35c0 = 59;
    g_4b43c0->field_35bc = g_4b43c0->field_35bc | 4;
    return;
}
