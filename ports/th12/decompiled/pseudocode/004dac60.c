// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dac60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_45e292d1;

int sub_4dac60(void)
{
    unsigned int *v8;  // edi
    unsigned int v9;  // eax
    unsigned int v18;  // ebx
    unsigned int v19;  // esi
    unsigned long v20;  // ldt
    unsigned long v21;  // gdt
    unsigned long long v22;  // 4142
    unsigned short v23;  // ss
    unsigned int *v10;  // edi
    unsigned int v11;  // cc_op
    unsigned int v12;  // cc_dep1
    unsigned int v13;  // cc_dep2
    unsigned int v14;  // cc_ndep
    unsigned int v15;  // 4112
    unsigned int v16;  // 4101
    unsigned int v17;  // edx
    unsigned int *v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    char *v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]
    unsigned short v6;  // [bp+0x0], Other Possible Types: unsigned int
    unsigned int v7;  // [bp+0x4]

    *(v8) = v9;
    v10 = v8 + 1;
    g_45e292d1 = v9;
    v15 = _ccall(14, v11, v12, v13, v14);
    if (!(v15 & 1))
        syscall(v10);
    v16 = _ccall(2, v11, v12, v13, v14);
    if (v16 & 1)
    {
        v6 = v23;
        *(v10) = v9;
    }
    v6 = v9;
    v5 = v6;
    v4 = v17;
    v3 = v18;
    v2 = &v7;
    v1 = v19 + 4;
    v0 = (char *)v10 + 3;
    v22 = _ccall(v20, v21, 38747, 650885612);
    goto *((void *)((unsigned int)v22));
}
