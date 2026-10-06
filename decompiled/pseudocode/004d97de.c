// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d97de; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d97de(void)
{
    unsigned short v4;  // es
    void* v5;  // ebx
    unsigned int v14;  // 4127
    unsigned int v15;  // ebp
    void* v6;  // ebx
    void* v7;  // edi
    void* v8;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // esi
    unsigned int v11;  // eax
    unsigned int v12;  // 4126
    char v13;  // dl
    unsigned int *v0;  // [bp-0x5]
    unsigned int v1;  // [bp+0x0]
    char v2;  // [bp+0x1]

    *((unsigned short *)((char *)&v0 + 1)) = v4;
    v6 = v5 - 1;
    v8 = v7 + 4;
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = _INSERT(v9 ^ 2026635290, 0, (char)(v9 ^ 2026635290) + 11 + (*((int *)v10) < *((int *)v7)));
    if (v11 == 2280371765)
        goto LABEL_0x4d978d;
    __unsupported_jumpkind_Ijk_Privileged()
    __unsupported_jumpkind_Ijk_Privileged()
    v12 = _ccall(3, 4129857617, *((int *)v6), 0);
    v13 = 4129857617 + *((int *)v6) - 1;
    v14 = _ccall(8, 18, &v2, 0, v12);
    if (v14 & 1)
        goto LABEL_0x4d980d;
    *(v0) = *(v0) + 1370396708;
    v1 = 0xffffff99;
    *((unsigned int *)v8) = *((int *)v8) - v15;
    *((char *)v8) = *((char *)v8) & v13;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
}
