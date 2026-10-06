// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4752ca; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adac8;
extern unsigned int g_4adacc;
extern unsigned int g_4b410c;

void sub_4752ca(void)
{
    unsigned int *v1;  // eax

    if (g_4adac8 != 0xffffffff)
    {
        v1 = sub_4751de(g_4b410c);
        v1(g_4adac8);
        g_4adac8 = 0xffffffff;
    }
    if (g_4adacc != 0xffffffff)
    {
        TlsFree(g_4adacc);
        g_4adacc = 0xffffffff;
    }
    sub_46eb53();
    return;
}
