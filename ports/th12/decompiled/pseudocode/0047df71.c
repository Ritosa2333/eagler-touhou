// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47df71; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49dde0;

unsigned int * sub_47df71(unsigned int *idx, unsigned int a1, unsigned int a2, unsigned int a3)
{
    idx[3] = 0xffffffff;
    idx[1] = a2;
    *(idx) = &g_49dde0;
    idx[2] = a3;
    return idx;
}
