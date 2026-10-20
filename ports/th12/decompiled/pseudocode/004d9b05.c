// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9b05; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_8b35f537;

int sub_4d9b05(void)
{
    char *v2;  // ecx
    char v3;  // 4102
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // eax
    unsigned int v8;  // 4103
    unsigned int v9;  // 4109
    unsigned int v10;  // eax
    unsigned int v0;  // [bp+0x0]

    v3 = *(v2);
    *(v2) = v3 - *((char *)((void*)&v2 + 1));
    v4 = v3;
    v5 = *((char *)((void*)&v2 + 1));
    if (v3 < *((char *)((void*)&v2 + 1)))
    {
        g_8b35f537 = _INSERT(v6, 0, 97);
    }
    else if ((char)v4 > (char)v5)
    {
        v8 = _ccall(4, v4, v5, 0);
        v9 = _ccall(10, 21, v0 - 1, 0, v8);
        if (v9 & 1)
            goto LABEL_0x4d9b70;
        else
            goto LABEL_0x4d9af7;
    }
    else
    {
        *((char *)(v10 - 999066990)) = *((char *)(v10 - 999066990)) & 45;
        x86g_dirtyhelper_OUT(89<32>, Conv(8->32, (Extract(vvar_0{r8|4b}, 8bits@0<64>) Or 157<8>)), 1<32>)
        return;
    }
}
