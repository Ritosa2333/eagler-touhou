// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4395d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4395d0(void)
{
    unsigned int *v1;  // edx
    unsigned int v2;  // esi
    unsigned int *idx;  // ecx
    unsigned int v4;  // ecx
    unsigned int *v5;  // eax

    v2 = *(v1);
    *(idx) = v2;
    idx[1] = v1[1];
    v4 = idx[1];
    *(v5) = v2;
    v5[1] = v4;
    return;
}
