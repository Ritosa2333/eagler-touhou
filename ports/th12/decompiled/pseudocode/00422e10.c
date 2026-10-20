// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422e10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b0ca0;
extern int g_4b0ca4;
extern int g_4b43e4;

int sub_422e10(void)
{
    void* idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // ecx

    v2 = (int)idx[96];
    if (v2 >= 9)
    {
        *((int *)&idx[100]) = 0;
        return;
    }
    *((unsigned int *)&idx[100]) = (int)idx[100] + 2;
    if ((int)idx[100] >= 5)
    {
        v3 = v2 + 1;
        *((int *)&idx[100]) = 0;
        *((unsigned int *)&idx[96]) = v3;
        if (v3 > 9)
            *((unsigned int *)&idx[96]) = 9;
        else
            sub_453d90(v3, 48);
        sub_41cf40(g_4b43e4, g_4b0ca0, g_4b0ca4);
    }
    sub_41cf40(g_4b43e4, g_4b0ca0, g_4b0ca4);
    return;
}
