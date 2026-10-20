// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db507; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_9f1b821e;

int sub_4db507(void)
{
    char v1;  // ch
    char v2;  // bh
    unsigned int v11;  // edi
    unsigned short v12;  // dx
    unsigned short v13;  // dx
    unsigned int v14;  // eax
    char v15;  // cl
    unsigned int v16;  // 4156
    unsigned int idx;  // esi
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // eax
    unsigned short v6;  // ftop
    unsigned int v7;  // fc3210
    unsigned int v8;  // 4111
    unsigned int v9;  // 4101
    char v10;  // cl

    __unsupported_jumpkind_Ijk_NoDecode()
    if (v1 > v2)
        goto LABEL_0x4db490;
    v4 = idx + 1;
    __unsupported_jumpkind_Ijk_NoDecode()
    *((unsigned short *)(v5 - 324327336)) = (v6 & 7) * 0x800 | (unsigned short)v7 & 0x4700;
    v8 = _ccall(6, 18, v4, 0, 0);
    if (v8 & 1)
    {
        v14 = _INSERT(v5, 0, x86g_dirtyhelper_IN(Conv(16->32, vvar_11{r16|2b}), 1<32>));
        v16 = _ccall((v15 & 31 ? 22 : 18), (v15 & 31 ? *((char *)((void*)&v13 + 1)) << (v15 & 31) : v4), (v15 & 31 ? *((char *)((void*)&v13 + 1)) << ((v15 & 31) - 1 & 31) : 0), 0);
        *((unsigned int *)(idx + 1708845032)) = *((int *)(idx + 1708845032)) - v14 - (v16 & 1);
    }
    v9 = _ccall(4, 18, v4, 0, 0);
    if (v9 & 1)
        goto LABEL_0x4db475;
    if (v10 & 31)
        g_9f1b821e = g_9f1b821e << (v10 & 31);
    else
        g_9f1b821e = g_9f1b821e << (v10 & 31);
    InterlockedExchange8(v11 - 100, *((char *)((void*)&v12 + 1)));
}
