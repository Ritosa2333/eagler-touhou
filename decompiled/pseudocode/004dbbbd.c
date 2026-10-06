// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbbbd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dbbbd(void)
{
    unsigned short v2;  // ftop
    unsigned int v3;  // fc3210
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4111
    unsigned int v9;  // eax

    *((unsigned short *)(_INSERT(0, 0, 110) + 0x7f9bf0c6)) = (v2 & 7) * 0x800 | (unsigned short)v3 & 0x4700;
    v8 = _ccall(2, v4, v5, v6, v7);
    if (v8 & 1)
        goto LABEL_0x4dbbe4;
    *((int *)(v9 - 673438569)) = *((int *)(v9 - 673438569)) >> 25;
}
