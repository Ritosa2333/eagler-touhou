// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d94ca; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4d94ca(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4129
    char *v7;  // eax
    char *v0;  // [bp-0x4]

    v6 = _ccall(v2, v3, v4, v5);
    *(v7) = *(v7) + *((char *)&v7) + ((char)v6 & 1);
    *(v7) = *(v7) + *((char *)&v7);
    v0 = v7;
    *(v7) = *(v7) + *((char *)&v7);
    *((char *)(v7 * 2)) = *((char *)(v7 * 2)) + *((char *)&v7);
    *(v7) = *(v7) + *((char *)&v7);
    *(v7) = *(v7) - *((char *)&v7);
}
