// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da42e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da42e(unsigned int a0)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int *v11;  // edi
    unsigned int v12;  // esi
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4109
    unsigned int v6;  // ebx
    unsigned int v7;  // cc_dep1
    unsigned int v8;  // 4120
    unsigned int v9;  // 4106
    unsigned int v10;  // edi

    v5 = _ccall(v1, v2, v3, v4);
    v7 = v6 + 1;
    v8 = _ccall(5, 18, v6 + 1, 0, v5);
    if (a0 != 1 & v8 & 1)
        goto LABEL_0x4da3da;
    if (!((v7 == 0x80000000 ? 1 : 0) & 1))
        goto LABEL_0x4da419;
    v9 = _ccall(4, 18, v7, 0, v5);
    if (a0 != 2 & v9 & 1)
    {
        *(v11) = *(v11) & v12 + 4;
        __debugbreak();
    }
    *((char *)(3994757763 + v10 * 8 - 101)) = *((char *)(3994757763 + v10 * 8 - 101)) * 2;
}
