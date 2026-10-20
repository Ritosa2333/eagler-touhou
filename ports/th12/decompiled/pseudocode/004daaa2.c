// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daaa2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_63868239;
extern unsigned int g_7441e816;

int sub_4daaa2(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v10;  // eax
    unsigned int v11;  // ebp
    unsigned int *v12;  // ebx
    unsigned short v13;  // ss
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // 4108
    unsigned int v17;  // esi
    unsigned int v18;  // edi
    unsigned int v3;  // cc_dep2
    unsigned int v19;  // 4148
    unsigned int v20;  // eax
    char v21;  // 4116
    unsigned int v22;  // 4170
    unsigned int v23;  // 4184
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4106
    unsigned int v6;  // ecx
    unsigned int iter;  // ecx
    unsigned int v8;  // eax
    unsigned int v9;  // edx

    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_18{r16|4b}, 16bits@0<64>)), Conv(8->32, Extract(vvar_19{r8|4b}, 8bits@0<64>)), 1<32>)
    v5 = _ccall(6, v1, v2, v3, v4);
    if (!(v5 & 1))
        goto LABEL_0x4daa62;
    iter = v6 - 1;
    if (v6 != 1)
        __debugbreak();
    if (v8 != 3352064751)
        goto LABEL_4daad3;
    iter += 1;
    do
    {
        *((char *)v12 - 2076035235) = *((char *)v12 - 2076035235) * 2 | (char)(*((char *)v12 - 2076035235)) >> 7;
        *((unsigned short *)(esp - 4)) = v13;
        *((char *)(v10 - 794475381)) = *((char *)((void*)&v10 + 1));
        v14 = v11;
        v11 = v10;
        esp = esp - 4 - *((int *)(iter + 2125778965));
        g_7441e816 = g_7441e816 + v14;
        iter = _INSERT(iter, 0, (char)iter >> ((char)iter & 31 & 7) | (char)iter << 8 - ((char)iter & 31 & 7));
        v10 = _INSERT(v14, 0, *((char *)((void*)&iter + 1)));
LABEL_4daad3:
        v15 = v11;
        v11 = v10;
        v10 = _INSERT(v15, 0, (char)v15 - *((char *)((void*)&iter + 1)));
        v9 = _INSERT(v9, 1, 235);
        v16 = _ccall(0, 4, (char)v15, *((char *)((void*)&iter + 1)), 0);
    } while (!(v16 & 1));
    *((unsigned int *)(esp - 4)) = v10;
    *((unsigned int *)(esp - 8)) = iter;
    *((unsigned int *)(esp - 12)) = v9;
    *((unsigned int **)(esp - 16)) = v12;
    *((int *)(esp - 20)) = esp;
    *((unsigned int *)(esp - 24)) = v11;
    *((unsigned int *)(esp - 28)) = v17;
    *((unsigned int *)(esp - 32)) = v18;
    v19 = _ccall(4, (char)v15, *((char *)((void*)&iter + 1)), 0);
    *((unsigned int *)(iter - 1293662107)) = *((int *)(iter - 1293662107)) + v18 + (v19 & 1);
    v20 = v10 + 1697024716;
    esp = *(v12) * 1124862193;
    v21 = *((char *)(v11 + v20 * 8 + 8));
    v22 = _ccall(39, *(v12), 1124862193, 0);
    *((char *)(v11 + v20 * 8 + 8)) = *((char *)(v11 + v20 * 8 + 8)) + 37 + ((char)v22 & 1);
    v23 = _ccall(7, v21, 37 ^ (char)v22 & 1, v22 & 1);
    g_63868239 = g_63868239 - esp - (v23 & 1);
}
