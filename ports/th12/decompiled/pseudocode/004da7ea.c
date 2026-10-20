// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da7ea; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4994803;

int sub_4da7ea(void)
{
    unsigned int v2;  // ebx
    char *v3;  // esi
    unsigned int v12;  // fpround
    unsigned int v13;  // 4109
    void* v14;  // ebp
    unsigned int v15;  // 4101
    unsigned int v16;  // esi
    unsigned int *idx;  // ecx
    unsigned int v19;  // eax
    char v20;  // 4115
    unsigned int v21;  // 4120
    char *v4;  // eax
    unsigned int v22;  // 4101
    unsigned int *v24;  // eax
    unsigned int v25;  // 4104
    unsigned int v26;  // 4108
    unsigned int v5;  // eax
    unsigned int v6;  // esi
    void* v7;  // edi
    char *v8;  // edi
    char v9;  // 4106
    unsigned int v10;  // 4145
    char v11;  // 4098
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    v4 = v3;
    v6 = v5;
    v8 = v7 & *((int *)((char *)v7 - 84));
    g_4994803 = v4;
    v9 = *(v4);
    InterlockedExchange8(v4 - 59, *((char *)((void*)&v2 + 1)));
    v10 = _ccall(10, 7, 254, v9 ^ 0, 0);
    if (!(v10 & 1))
    {
        v11 = *(v8 - 595552859);
        v13 = _ccall(v12);
        *((unsigned short *)((char *)v14 - 76)) = v13;
        if (v11 <= 254 + v9)
            goto LABEL_0x4da845;
        else
            goto LABEL_0x4da7d4;
    }
    else
    {
        v15 = _ccall(10, 7, 254, v9 ^ 0, 0);
        if (!(v15 & 1))
            __debugbreak();
        while (1)
        {
            v16 = v6;
            *(idx) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v19 = _INSERT(_INSERT(v4, 1, 0), 0, *((char *)&v4) + *((char *)((void*)&v4 + 1)) * 108);
            v20 = *(v8);
            *(v8) = v20 * 0;
            v21 = _ccall(12, 22, v20 * 0x200, v20 * 0x100, 0);
            if (!(v21 & 1))
            {
                v22 = _ccall(2, 22, v20 * 0x200, v20 * 0x100, 0);
                if (v22 & 1)
                    *((char *)&idx[384171028] + 3) = *((char *)&idx[384171028] + 3) | *((char *)((void*)&v19 | 245 + 1));
                v24 = _INSERT(_INSERT(v19, 1, (char)v19 / 250), 0, (char)v19 % 250);
                v25 = *(v24);
                v6 = v16 ^ v25;
                v4 = v24 & 4176996586;
                v26 = *(idx);
                *(idx) = *(idx) | v16 ^ v25;
            }
        }
    }
}
