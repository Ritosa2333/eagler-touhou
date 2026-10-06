// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422290; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b0ce0;
extern unsigned int g_4b2ed0;
extern unsigned int g_4b4508;
extern unsigned int g_4b450c;
extern unsigned int g_4cee40;

void sub_422290(void)
{
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebp
    int v4;  // ebx

    sub_43d2e0(v1, v2, v3, v4);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b0ce0 = g_4b0ce0 & 0xfffffffc;
    g_4b450c = 0;
    g_4b4508 = 0;
    if (g_4cee40 != 10 && g_4cee40 != 11)
    {
        if (g_4cee40 == 12)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4222db();
            return;
        }
        if (g_4cee40 != 4 && g_4cee40 != 16)
        {
            if (g_4cee40 != 14)
                goto LABEL_0x422397;
            /* unsupported instruction */
            /* unsupported instruction */
            sub_42232d();
            return;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        sub_422306();
        return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    sub_422368();
    return;
}
