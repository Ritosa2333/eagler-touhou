// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dae6f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dae6f_0 {
    char padding_0[22];
    char field_16;
} st_4dae6f_0;

extern unsigned int g_fb69875e;

int sub_4dae6f(void)
{
    unsigned int v6;  // eax
    unsigned int v7;  // ecx
    int v8;  // cc_dep1
    unsigned int v9;  // 4101
    unsigned short v10;  // ss
    st_4dae6f_0 *v11;  // ecx
    unsigned int idx;  // ebp
    char v13;  // 4100
    unsigned int v14;  // 4138
    unsigned int v0;  // [bp+0x0], Other Possible Types: unsigned short
    unsigned int v2;  // [bp+0x8]
    unsigned int v3;  // [bp+0x14]
    st_4dae6f_0 *v4;  // [bp+0x18], Other Possible Types: unsigned short
    st_4dae6f_0 *v5;  // [bp+0x1c], Other Possible Types: int, unsigned int, unsigned short

    v8 = v6 ^ v7;
    if (v7 != 1)
    {
        if (v8 < 0)
            goto LABEL_0x4dae64;
        else
            goto LABEL_0x4dae13;
    }
    else
    {
        v9 = _ccall(0, 15, v8, 0, 0);
        if (!(v9 & 1))
            v0 = v10;
        v11 = v4;
        v5 = v3;
        idx = v2 + 1;
        v11->field_16 = -(v11->field_16);
        __unsupported_jumpkind_Ijk_Privileged()
        v13 = v11[5].padding_0[3 + 2 * v0];
        v11[5].padding_0[3 + 2 * v0] = v11[5].padding_0[3 + 2 * v0] + 97;
        InterlockedExchange((v5 >> 31) - 118, v0);
        v5 = v11;
        v14 = _ccall(10, 4, v13, 159, 0);
        if (v14 & 1)
            goto LABEL_0x4dae95;
        g_fb69875e = g_fb69875e ^ &v5;
        if (/* unsupported instruction */)
            *((short *)(idx + 213065771)) = /* unsupported instruction */;
        else
            *((short *)(idx + 213065771)) = nan;
        v5 = *((unsigned short *)&v5);
        v4 = v5;
    }
}
