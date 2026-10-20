// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dacf5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dacf5(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    void* v12;  // ecx
    unsigned int v13;  // 4120
    unsigned int v14;  // 4101
    unsigned int v15;  // edx
    unsigned int v16;  // 4111
    unsigned int v17;  // edx
    unsigned long long v18;  // 4112
    char v19;  // al
    unsigned int v20;  // ebx
    unsigned int v21;  // ebp
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4106
    unsigned int v7;  // cc_ndep
    void* v8;  // edi
    void* v9;  // cc_dep1
    char *v10;  // esi
    void* v11;  // ecx
    unsigned int v0;  // [bp+0x0]

    v6 = _ccall(v2, v3, v4, v5);
    v7 = v6;
    v9 = v8 - 1;
    *((char *)v8 - 1) = *(v10);
    v12 = v11 - 1;
    v13 = _ccall(5, 21, v8 - 1, 0, v6);
    if (!(v11 != 0x1 & v13 & 1))
        InterlockedExchange8(v20 + v21, v0);
    v14 = _ccall(10, 21, v9, 0, v7);
    if (v14 & 1)
    {
        v16 = _ccall(21, v9, 0, v7);
        v18 = _ccall(v17, *((char *)&v12), v16, 4);
        *((char *)v12 + 8 * v18 - 1246985816) = v19;
    }
    *((char *)v12) = *((char *)v12) - (char)v15;
    __debugbreak();
}
