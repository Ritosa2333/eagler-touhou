// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d987e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4d987e(unsigned int a0, unsigned int a1)
{
    char v1;  // 4134
    unsigned int v2;  // ebx
    unsigned int v3;  // eax
    unsigned int v4;  // ebp

    v1 = *((char *)(a0 + a1 - 31));
    v3 = _INSERT(v2, 1, *((char *)(a0 + a1 - 31)));
    *((unsigned int *)(*((int *)0x7007e454) + 40508098)) = *((int *)(*((int *)0x7007e454) + 40508098)) & v4;
    if (*((char *)(v4 - 1256023701)) >= v1)
        goto LABEL_0x4d989d;
    return v3;
}
