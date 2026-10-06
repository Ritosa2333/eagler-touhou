// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d96af; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d96af_0 {
    unsigned int field_0;
} st_4d96af_0;

extern unsigned int g_24223e20;

void sub_4d96af(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int *v0;  // eax
    unsigned int v8;  // 4113
    st_4d96af_0 **v9;  // eax
    void* v10;  // edx
    unsigned int v11;  // eax
    unsigned int v12;  // 4147
    unsigned int v13;  // 4149
    void* v15;  // edi
    void* v16;  // edi
    unsigned int v17;  // eax
    void* v1;  // esi
    unsigned int v18;  // ebx
    unsigned int *v19;  // edi
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // 4154
    unsigned long long v23;  // 4155
    unsigned int v24;  // 4110
    unsigned int v25;  // 4161
    unsigned int v26;  // 4157
    unsigned int v28;  // 4159
    unsigned int v29;  // 4161
    unsigned int v30;  // 4168
    st_4d96af_0 **idx;  // ebp
    unsigned int *v3;  // eax
    void* v4;  // esi
    unsigned int v5;  // 4102
    unsigned int v6;  // 4145
    unsigned int v7;  // 4112

    v0 = x86g_dirtyhelper_IN(201<32>, 4<32>);
    do
    {
        v3 = v0;
        v4 = v1 + ((unsigned int)(*((int *)esp)) >> 10 & 1 ? 0xffffffff : 1) * 4;
        v5 = *((int *)(esp + 4));
        v6 = _ccall(8, 0, v5 & 2261, 0, 0);
        if (!(v6 & 1))
            goto LABEL_0x4d964d;
        v7 = _ccall(0, v5 & 2261, 0, 0);
        v8 = _ccall(CONCAT((unsigned short)v7, (unsigned short)*((int *)v1)), 55);
        v9 = _INSERT(*((int *)v1), 0, v8);
        v1 = v4 ^ a0;
        esp = idx + 1;
        v0 = &*(idx)->field_0;
        idx = v9;
    } while ((v4 ^ a0) < 0);
    esp = esp - 4;
    *((int *)(esp - 4)) = esp;
    v10 = *((int *)v1) * 4;
    v11 = v0 & *(v0);
    *((unsigned int *)(esp - 4)) = 4071769694;
    v12 = _ccall(15, v11 & 2311850123, 0, 0);
    v13 = _ccall(CONCAT((unsigned short)v12, (unsigned short)v11), 55);
    v16 = v15 - 1;
    v17 = _INSERT(v11, 0, v13) + 1;
    if (a0 != 1 & v17 & 3138072512)
        goto LABEL_0x4d9707;
    v18 = _INSERT(v3, 1, *((char *)((void*)&v3 + 1)) ^ *((char *)v1));
    v19 = v16 + (v5 >> 10 & 1 ? 0xffffffff : 1);
    __unsupported_jumpkind_Ijk_Privileged()
    v20 = _INSERT(v17, 0, x86g_dirtyhelper_IN(245<32>, 1<32>));
    esp = esp;
    if (*((char *)v1) < *((char *)v16))
    {
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
        586067132();
    }
    else
    {
        v21 = v20 + 1759637600;
        esp = esp - 4;
        *((unsigned int *)(esp - 4)) = 0xffffffe5;
        *((unsigned int *)(esp - 4)) = v18;
        v22 = _ccall(6, v20, 2535329696, 0);
        v23 = _ccall(*((char *)v21), 135, v22, 1);
        *((char *)v21) = v23;
        esp = esp - 3;
        v24 = g_24223e20;
        g_24223e20 = v24 - (char *)v19 - ((unsigned int)(v23 >> 32) & 1);
        v25 = _ccall(8, 12, v24, v19 ^ (unsigned int)(v23 >> 32) & 1, (unsigned int)(v23 >> 32) & 1);
        if (v25 & 1)
        {
            v26 = _ccall(12, v24, v19 ^ (unsigned int)(v23 >> 32) & 1, (unsigned int)(v23 >> 32) & 1);
            v28 = _ccall(18, esp + 1, 0, v26);
            v29 = _ccall(CONCAT((unsigned short)v28, (short)x86g_dirtyhelper_IN(Conv(16->32, Extract(vvar_81{r16|4b}, 16bits@0<64>)), 4<32>)), 39);
            v30 = _ccall(7, v29 & 0xff, *(0x24223e54) ^ (char)(v29 >> 16) & 213 & 1, v29 >> 16 & 2261 & 1);
            *((unsigned int **)((char *)&idx[466970470] + 1)) = *((int *)((char *)&idx[466970470] + 1)) - (char *)v19 - (v30 & 1);
        }
        else
        {
            *((int *)(esp + 4)) = *((int *)(esp + 4));
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
            *(v19) = v21;
            *((char *)v10 - 126) = *((char *)v10 - 126) * 16 | (char)(*((char *)v10 - 126)) >> 4;
        }
    }
}
