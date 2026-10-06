// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9943; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4d9943(void* a0, unsigned int a1)
{
    unsigned int v8;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // ebx
    unsigned int v11;  // ebp
    unsigned int v12;  // esi
    unsigned int v13;  // 4123
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    void* v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

    *((unsigned int *)((char *)a0 - 75)) = *((int *)((char *)a0 - 75)) + v8;
    v6 = v9;
    v5 = a0;
    v4 = a1;
    v3 = v10;
    v2 = v11;
    v1 = v12;
    v0 = v8;
    v13 = _ccall(4, 9, *((int *)((char *)a0 - 75)), v8 ^ 0, 0);
    if (a0 != 0x1 & v13 & 1)
        goto LABEL_0x4d9945;
}
