// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc680; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4dc680(unsigned int *idx, unsigned int a1, unsigned int a2, unsigned int a3)
{
    idx[33] = a2;
    idx[0x22] = a3;
    idx[35] = a3 + a2 * 4;
    return a3 + a2 * 4 + 0x100;
}
