// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da832; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4da832(unsigned int a0, unsigned int a1)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // 4115
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4136
    unsigned int v7;  // ebx
    unsigned long long v8;  // 4137
    void* v9;  // esi
    unsigned long long v10;  // 4141
    unsigned int v11;  // 4144
    unsigned int v0;  // [bp+0x0]

    v6 = _ccall(v2, v3, v4, v5);
    v8 = _ccall(v7, 20, v6, 4);
    v10 = _ccall(*((char *)v9 - 0x22), 1, (unsigned int)(v8 >> 32) & 2261, 1);
    *((char *)v9 - 0x22) = v10;
    v11 = _ccall(0, 15, *((int *)0xf4662806) ^ 3108011085, 0, 0);
    if (v11 & 1)
        goto LABEL_0x4da818;
    v12 = _ccall(15, *((int *)0xf4662806) ^ 3108011085, 0, 0);
    *((unsigned int *)(v0 + a1 * 2 + 352824403)) = *((int *)(v0 + a1 * 2 + 352824403)) + 67 + (v12 & 1);
}
