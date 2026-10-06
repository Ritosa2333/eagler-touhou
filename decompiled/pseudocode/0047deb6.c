// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47deb6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49ddd4;

unsigned int * sub_47deb6(unsigned int *idx, unsigned int a1, unsigned int a2)
{
    idx[1] = a2;
    *(idx) = &g_49ddd4;
    idx[2] = (-(0 < a2 - 1) & 0xfffffffc) + 4;
    return idx;
}
