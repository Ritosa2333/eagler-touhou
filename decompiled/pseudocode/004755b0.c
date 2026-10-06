// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4755b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adac8;
extern unsigned int g_4adacc;
extern unsigned int g_4b4108;

void sub_4755b0(int a0)
{
    unsigned int *v0;  // eax
    unsigned int v1;  // esi
    unsigned int *v2;  // eax

    if (g_4adac8 != 0xffffffff)
    {
        if (!a0)
        {
            if (TlsGetValue(g_4adacc))
            {
                v0 = TlsGetValue(g_4adacc);
                a0 = v0(g_4adac8, v1);
            }
        }
        v2 = sub_4751de(g_4b4108);
        v2(g_4adac8, 0);
        sub_475481(a0);
    }
    if (g_4adacc != 0xffffffff)
        TlsSetValue(g_4adacc, NULL);
    return;
}
