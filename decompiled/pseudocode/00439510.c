// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439510; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_439510(void)
{
    int *v1;  // eax
    int v2;  // esi
    unsigned int v3;  // edx
    unsigned int *v4;  // ecx

    v2 = v1[1];
    v3 = 1374389535 * *(v1) >> 37;
    *(v4) = (v3 >> 31) + v3;
    v4[1] = ((unsigned int)((int)(1374389535 * v2 >> 37)) >> 31) + (int)(1374389535 * v2 >> 37);
    return;
}
