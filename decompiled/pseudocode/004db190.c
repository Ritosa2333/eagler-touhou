// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db190; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_eac6bb9f;

int sub_4db190(void)
{
    unsigned int *v2;  // edi
    unsigned int *v3;  // esi
    unsigned long v12;  // ldt
    unsigned long v13;  // gdt
    unsigned long long v14;  // 4116
    unsigned int *idx;  // edi
    char v5;  // al
    unsigned int v6;  // eax
    char v7;  // bl
    unsigned int v8;  // 4109
    unsigned short v9;  // ds
    unsigned int v10;  // edx
    unsigned int v11;  // edx
    unsigned short v0;  // [bp-0x4]

    *(v2) = *(v3);
    idx = v2 + 1;
    v6 = _INSERT(CONCAT(v5, 0), 0, v5 & 144);
    if ((v5 & 144) < 0)
    {
        v8 = _ccall(0, 4, *((char *)&v3[53065196] + 3), v7, 0);
        if (v8 & 1)
            goto LABEL_0x4db218;
        v0 = v9;
        v11 = _INSERT(v10 - 1, 1, *((char *)((void*)&v10 - 1 + 1)) ^ *(0x1bc3d107));
        *((unsigned int *)(g_eac6bb9f - 4)) = v11;
    }
    g_eac6bb9f = v6;
    if ((v5 & 144) < 0)
    {
        57294487();
        if (/* unsupported instruction */)
        {
            *((long long *)((char *)&idx[0xeddbfd0] + 2)) = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            *((long long *)((char *)&idx[0xeddbfd0] + 2)) = nan;
            /* unsupported instruction */
        }
        idx[20692550] = idx[20692550] * 2;
        return;
    }
    v14 = _ccall(v12, v13, 41854, 3684648047);
    goto *((void *)((unsigned int)v14));
}
