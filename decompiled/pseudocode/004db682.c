// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db682; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4db682(void)
{
    unsigned int v8;  // ecx
    void* v9;  // edi
    unsigned int v16;  // cc_dep1
    unsigned int v17;  // cc_dep2
    unsigned int v18;  // cc_ndep
    unsigned int v19;  // 4133
    unsigned int v10;  // eax
    unsigned int v11;  // edx
    unsigned int v12;  // ebx
    unsigned int v13;  // ebp
    unsigned int v14;  // esi
    unsigned int v15;  // cc_op
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

    if ((char)v8 & 31)
        *((int *)((char *)v9 - 104)) = *((int *)((char *)v9 - 104)) >> ((char)v8 & 31);
    else
        *((int *)((char *)v9 - 104)) = *((int *)((char *)v9 - 104)) >> ((char)v8 & 31);
    v6 = v10;
    v5 = v8;
    v4 = v11;
    v3 = v12;
    v2 = &v14;
    v1 = v13 - 1;
    v0 = v14;
    x86g_dirtyhelper_OUT(226<32>, vvar_8{r8|4b}, 4<32>)
    esp = __ROL__(&v9, 25) - 4;
    1865108845();
    esp = esp + 32;
    v19 = _ccall(v15, v16, v17, v18);
    if (!(*((int *)(esp + 24)) != 1 & !((v19 | 1) >> 6 & 1)))
    {
        *((int *)(esp - 4)) = *((int *)(esp + 4));
        __debugbreak();
    }
    if (*((int *)esp) == 1)
        goto LABEL_0x4db718;
}
