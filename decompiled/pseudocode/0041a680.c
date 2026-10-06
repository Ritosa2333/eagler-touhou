// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a680; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_41a680(void)
{
    unsigned int v1;  // eax
    unsigned int v2;  // edx
    unsigned int *idx;  // eax
    unsigned int v4;  // edi
    unsigned int v5;  // ecx

    idx = v1 * 16 + v2;
    *((unsigned int *)((v1 + 625) * 16 + v2)) = v4;
    idx[2501] = v5;
    idx[2502] = v5;
    return;
}
