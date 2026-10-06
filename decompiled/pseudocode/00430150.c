// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430150; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b451c;
extern char g_4ceae8;

unsigned int sub_430150(unsigned int a0)
{
    int v1;  // edi
    unsigned int v0;  // [bp-0x8]

    if (g_4ceae8 & 16)
    {
        v0 = 0;
        sub_454960(4, 0);
    }
    sub_454960(2, v1);
    *((char *)(g_4b451c + v0 + 125402)) = 1;
    return 0;
}
