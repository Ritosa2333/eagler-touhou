// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4356d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4356d0(unsigned int a0, int *a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7)
{
    unsigned int v0;  // eax
    int *v1;  // ecx
    unsigned int *idx;  // eax

    v0 = 0;
    v1 = a1;
    while (*(v1) >= 0)
    {
        v0 += 1;
        v1 += 10;
        if (v0 >= 4)
            return 0;
    }
    idx = &a1[10 * v0];
    *(idx) = a2;
    idx[1] = a3;
    idx[6] = a4;
    idx[7] = a5;
    idx[2] = 32;
    idx[3] = 16;
    idx[4] = 384;
    idx[5] = 448;
    idx[8] = a6;
    idx[9] = a7;
    return 0;
}
