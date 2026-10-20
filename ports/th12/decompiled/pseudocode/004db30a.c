// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db30a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db30a_0 {
    char padding_0[5];
    char field_5;
} st_4db30a_0;


int sub_4db30a(unsigned int a0, unsigned int a1)
{
    int v2;  // eax
    unsigned int *v3;  // ebx
    char *v12;  // esi
    unsigned int v4;  // ecx
    char v5;  // 4105
    unsigned int v6;  // 4124
    st_4db30a_0 *v7;  // edi
    unsigned short v8;  // es
    unsigned int v9;  // 4110
    unsigned int v10;  // 4124
    unsigned int v11;  // 4117
    int v0;  // [bp+0x0], Other Possible Types: unsigned short

    if (!a0)
        goto LABEL_0x4db28c;
    v2 = v0;
    v4 = (a0 & *(v3)) - 1;
    v5 = *((char *)(a1 - 1843072123));
    *((char *)(a1 - 1843072123)) = *((char *)(a1 - 1843072123)) - *((char *)((void*)&a1 + 1));
    v6 = _ccall(0, 4, v5, *((char *)((void*)&a1 + 1)), 0);
    if (!(v6 & 1))
        sub_4db2e4(); /* do not return */
    if (v2 < *((int *)&v7->padding_0[0]))
        goto LABEL_0x4db319;
    v0 = v8;
    v9 = _ccall(6, v2, *((int *)&v7->padding_0[0]), 0);
    v10 = _ccall(5, 12, v2, 2102679021 ^ v9 & 1, v9 & 1);
    if (v4 != 1 & v10 & 1)
    {
        if (v4 != 1)
        {
            v11 = _ccall(12, 0, CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) & 69, 0, 0);
            if (v11 & 1)
                goto LABEL_0x4db37a;
            v7->field_5 = *(v12);
        }
        __unsupported_jumpkind_Ijk_NoDecode()
    }
}
