// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_439560(void)
{
    unsigned int *v1;  // ecx
    unsigned int *v2;  // edx
    unsigned int v3;  // ecx
    unsigned int *v4;  // eax

    v3 = v1[1] + v2[1];
    *(v4) = *(v1) + *(v2);
    v4[1] = v3;
    return;
}
