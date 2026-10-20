// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45ed20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4a2338[4];
extern char g_4ceae8;

int sub_45ed20(void)
{
    unsigned int idx;  // eax
    unsigned int *v2;  // ecx

    if (!(g_4ceae8 & 1))
        return;
    v2 = g_4a2338[idx];
    if (v2 != 0x15 && v2)
        return;
    return;
}
