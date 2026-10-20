// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422f20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b0ca0;
extern int g_4b0ca4;
extern int g_4b43e4;

int sub_422f20(void)
{
    int v1;  // eax
    int v2;  // eax

    v1 = g_4b0ca0;
    v2 = v1 - 1;
    g_4b0ca0 = v2;
    if (v1 - 1 < 0)
    {
        v2 = 0;
    }
    else
    {
        if (v2 <= 8)
            goto LABEL_422f42;
        v2 = 8;
    }
    g_4b0ca0 = v2;
LABEL_422f42:
    if (g_4b43e4)
        v2 = sub_41cf40(g_4b43e4, v2, g_4b0ca4);
    return v2;
}
