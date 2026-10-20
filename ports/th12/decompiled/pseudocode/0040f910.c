// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40f910; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43d4;

int sub_40f910(void)
{
    unsigned int *v1;  // eax

    memset(v1, 0, 36);
    *(v1) = *(v1) | 2;
    g_4b43d4 = v1;
    return;
}
