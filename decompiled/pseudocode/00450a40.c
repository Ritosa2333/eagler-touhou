// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450a40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4a2664;
extern unsigned int g_4ce8ec;

unsigned int sub_450a40(void)
{
    int v1;  // edi

    g_4ce8ec = (unsigned int)Direct3DCreate9(32);
    if (g_4ce8ec)
        return 0;
    sub_464300(&g_4a2664, v1);
    return 1;
}
