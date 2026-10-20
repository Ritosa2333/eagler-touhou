// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da4a2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_dbd911db;

void sub_4da4a2(char a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v10;  // ebp
    void* v11;  // edi
    unsigned int v12;  // eax
    unsigned int v13;  // cc_dep1
    unsigned int v14;  // 4129
    unsigned int v15;  // eax
    unsigned int v16;  // 4103
    unsigned int v17;  // 4101
    char v18;  // bl
    unsigned int v19;  // 4123
    unsigned int v3;  // cc_dep2
    char *v20;  // esi
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4117
    unsigned int v6;  // eax
    char v7;  // 4096
    unsigned int v8;  // 4106
    unsigned int v9;  // 4126
    unsigned int v0;  // [bp+0x0]

    v5 = _ccall(v1, v2, v3, v4);
    v6 = _INSERT(0, 0, (char)((int)((v5 & 1) * 0x80000000) >> 31) ^ 25);
    if ((char)((int)((v5 & 1) * 0x80000000) >> 31) ^ 25)
    {
        v7 = g_dbd911db;
        v8 = g_dbd911db;
        g_dbd911db = g_dbd911db >> 1;
        v9 = _ccall(6, 25, v8 >> 1, v7, 0);
        if (!(v9 & 1))
            x86g_dirtyhelper_OUT(71<32>, Conv(8->32, ((Extract(vvar_30{r8|4b}, 8bits@0<64>) Xor 208<8>) Or 219<8>)), 1<32>)
    }
    else
    {
        while (1)
        {
            v12 = CONCAT(a1, v6) / (unsigned long long)v11;
            v13 = v0 & 2261;
            x86g_dirtyhelper_OUT(255<32>, vvar_78{r8|4b}, 4<32>)
            v14 = _ccall(12, 0, v0 & 2261, 0, 0);
            if (!(v14 & 1))
            {
                v19 = _ccall(4, *((char *)(v10 + 208463376)), *((char *)((void*)&v12 + 1)), 0);
                /* unsupported instruction */
                /* unsupported instruction */
                *((char *)v11) = *(v20);
                break;
            }
            else
            {
                v15 = v10;
                a1 = 0xe5febe14;
                v16 = _ccall(12, 0, v13, 0, 0);
                if (v16 & 1)
                    goto LABEL_0x4da529;
                v17 = _ccall(12, 0, v13, 0, 0);
                if (v17 & 1)
                    break;
                *((char *)v11 - 22) = *((char *)v11 - 22) ^ v18;
                x86g_dirtyhelper_OUT(248<32>, Conv(8->32, Extract(vvar_89{r8|4b}, 8bits@0<64>)), 1<32>)
                v6 = _INSERT(v15, 0, x86g_dirtyhelper_IN(48660<32>, 1<32>));
                v10 = a2;
            }
        }
        x86g_dirtyhelper_OUT(95<32>, Conv(8->32, (Conv(32->8, vvar_78{r8|4b}) Or (((Conv(32->8, vvar_125) And 213<8>) Or 2<8>) Mul 0<8>))), 1<32>)
    }
}
