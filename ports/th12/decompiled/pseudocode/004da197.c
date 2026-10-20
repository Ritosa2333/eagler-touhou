// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da197; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4da197_1 {
    unsigned int field_0;
} st_4da197_1;

typedef struct st_4da197_2 {
    char padding_0[1];
    unsigned int field_1;
} st_4da197_2;

typedef struct st_4da197_0 {
    unsigned int field_0;
    struct st_4da197_1 *field_4;
} st_4da197_0;

extern char g_ac43d081;
extern char g_ea1267c9;

int sub_4da197(void)
{
    unsigned int *idx;  // ecx
    unsigned int v17;  // d
    unsigned int *v18;  // eax
    st_4da197_0 *v19;  // edi
    st_4da197_2 *v20;  // ebx
    st_4da197_2 *index;  // ebx
    unsigned int v22;  // ebp
    unsigned int v23;  // eax
    char v24;  // 4122
    unsigned int v25;  // 4178
    unsigned int v26;  // 4110
    unsigned int v27;  // ebp
    unsigned int v28;  // 4136
    unsigned int v29;  // 4105
    unsigned int v30;  // 4099
    unsigned short v31;  // ax
    unsigned int v13;  // eax
    st_4da197_0 *v14;  // edi
    void* iter;  // esi
    unsigned int v16;  // edx
    st_4da197_0 *v0;  // [bp-0xd]
    unsigned int v1;  // [bp-0x9]
    st_4da197_0 *v2;  // [bp-0x5]
    unsigned int v4;  // [bp+0x17]
    st_4da197_0 *v5;  // [bp+0x1b]
    void* v6;  // [bp+0x1f]
    unsigned int v7;  // [bp+0x23]
    unsigned int v8;  // [bp+0x2b]
    unsigned int v9;  // [bp+0x2f]
    unsigned int v10;  // [bp+0x33]
    st_4da197_2 *v11;  // [bp+0x37]

    while (1)
    {
        v19->field_0 = v18;
        v14 = &(&v19->field_0)[v17];
        v13 = _INSERT(v18 ^ 103, 1, 18);
        if ((v16 & *((int *)0x32709460)) >= 0)
        {
            v17 = 1;
            if (!idx)
                goto LABEL_0x4da15c;
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = v13 & 0x7f98cfde;
            v14->field_0 = *((int *)iter);
            v19 = &v14->field_4;
            iter += 4;
            idx[28] = idx[28] >> (*((char *)&idx) & 31);
            index = v20;
        }
        else
        {
            x86g_dirtyhelper_OUT(231<32>, vvar_52{r8|4b}, 4<32>)
            v14->field_0 = v14->field_0 | 867700274;
            v2 = v14;
            *((unsigned long *)(index->padding_0)[1]) = *((int *)(index->padding_0)[1]) ^ v14;
            g_ac43d081 = g_ac43d081 * 2;
            v1 = 867700274;
            __unsupported_jumpkind_Ijk_Privileged()
            v0 = v14;
            v17 = 0xffffffff;
            do
            {
                do
                {
                    v16 = v9;
                    v22 = v7;
                    iter = v6;
                    v19 = v5;
                    idx = _INSERT(v10, 1, *((char *)((void*)&v10 + 1)) ^ (char)iter[90]);
                    v23 = v8;
                    index = v11;
                    v24 = *((char *)iter - 42);
                    *((char *)iter - 42) = v24 + (char)v23;
                    v25 = _ccall(4, 7, v24, (char)v23 ^ 0, 0);
                } while (!(v25 & 1));
            } while (!idx);
            v26 = _ccall(7, v24, (char)v23 ^ 0, 0);
            v18 = v23 + 753110444 + (v26 & 1);
            v27 = v22 - *(v18);
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = _ccall(0, 6, v22, *(v18), 0);
            if (v28 & 1)
            {
                v29 = _ccall(0, 15, v27 ^ *(idx), 0, 0);
                if (!(v29 & 1))
                    goto LABEL_0x4da1a3;
                v30 = x86g_dirtyhelper_IN(Conv(16->32, Extract(vvar_60{r16|4b}, 16bits@0<64>)), 1<32>);
                v31 = _INSERT(v27 ^ *(idx), 0, v30);
                *((char *)&v19->field_0) = v30;
                g_ea1267c9 = v30;
                *((char *)&iter[83]) = (char)iter[83] + *((char *)((void*)&v31 + 1));
                *((char *)iter + ((int)(v4) >> 3) - 339117220) = *((char *)iter + ((int)(v4) >> 3) - 339117220) ^ 1 << ((char)v4 & 7);
                __debugbreak();
            }
        }
    }
}
