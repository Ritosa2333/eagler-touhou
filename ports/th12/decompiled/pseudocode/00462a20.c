// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462a20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_462a20(void)
{
    short v1;  // ax
    unsigned int *v2;  // esi
    char *v3;  // edx
    unsigned int v4;  // ecx

    if (v1 >= 0)
        *(v2) = *(v2) | -(0 < (v3[v1] & 128)) & v4;
    return;
}
