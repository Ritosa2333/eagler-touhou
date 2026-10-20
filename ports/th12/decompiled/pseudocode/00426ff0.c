// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x426ff0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b0c44;
extern int g_4b0c48;
extern int g_4b0ccc;
extern char g_4b0cd0;
extern int g_4b4514;

int sub_426ff0(void)
{
    int v3;  // esi
    int v4;  // ebx
    int v5;  // ecx
    int v6;  // eax
    int v0;  // [bp-0x1c]

    if (g_4b0c48 >= *((int *)&g_4b0cd0))
    {
        g_4b0c44 = g_4b0c44 + 1;
        if (g_4b0c44 >= 1000000000)
            g_4b0c44 = 0x3b9ac9ff;
        sub_43e250(v6 + 2412, -0x1);
        return;
    }
    else if (sub_422d70(v3, v4, v5))
    {
        sub_4385b0(g_4b4514);
        sub_43e250(v6 + 2412, -0xc0);
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v5;
        /* unsupported instruction */
        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        g_4b0ccc = g_4b0ccc + 12;
        if (g_4b0ccc > 0x400)
        {
            g_4b0ccc = 0x400;
            return;
        }
        if (g_4b0ccc >= -0x400)
            return;
        g_4b0ccc = 0xfffffc00;
        return;
    }
    else
    {
        return;
    }
}
