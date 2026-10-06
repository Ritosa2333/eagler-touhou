// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9d31; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d9d31_1 {
    char padding_0[4];
    unsigned int field_4;
} st_4d9d31_1;

typedef struct st_4d9d31_0 {
    char padding_0[1];
    unsigned int field_1;
} st_4d9d31_0;

extern unsigned int g_c30b0dc8;
extern unsigned int g_e95eb275;
extern unsigned int g_e95eb278;

int sub_4d9d31(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // ebx
    unsigned int v13;  // 4149
    char *v14;  // ebp
    unsigned int v15;  // 4101
    unsigned int v16;  // 4106
    unsigned int v17;  // cc_op
    unsigned int v18;  // cc_dep2
    unsigned int v19;  // cc_dep1
    unsigned int v20;  // 4101
    unsigned int v21;  // ebx
    unsigned int v5;  // esi
    void* iter;  // edi
    unsigned int v23;  // 4180
    char *v24;  // edx
    unsigned int v25;  // 4181
    unsigned int v26;  // 4183
    char v27;  // al
    unsigned int v28;  // ebp
    unsigned int v30;  // ebx
    unsigned int v31;  // 4146
    st_4d9d31_0 *v6;  // ecx
    unsigned int v32;  // 4105
    st_4d9d31_0 *v7;  // ecx
    char v8;  // al
    st_4d9d31_1 *idx;  // eax
    st_4d9d31_1 *v10;  // eax
    char *v11;  // edx
    unsigned int v12;  // cc_dep1
    char v0;  // [bp-0x4]
    char v1;  // [bp+0x0]

    v5 = v3 - v4;
    v7 = (v6->padding_0)[1];
    idx = _INSERT(CONCAT(v8, 0), 0, v8 - 14 - (v3 < v4));
    idx->field_4 = idx->field_4 - v5;
    v10 = (char *)idx - 1682344302;
    v12 = *((char *)((void*)&v11 + 1)) & *((char *)((void*)&v4 + 1));
    g_c30b0dc8 = v10;
    v13 = _ccall(14, 13, *((char *)((void*)&v11 + 1)) & *((char *)((void*)&v4 + 1)), 0, 0);
    if (!(v13 & 1))
        *((st_4d9d31_1 **)v7->padding_0) = &v10->padding_0[*((int *)v7->padding_0) + (v14 < &v1)];
    v15 = _ccall(8, 13, v12, 0, 0);
    if (!(v15 & 1))
    {
        esp = &v0;
        v16 = _ccall(13, v12, 0, 0);
        v17 = 0;
        v18 = 0;
        v19 = v16 & 0xfffffffe;
        v20 = _ccall(0, 0, v19, 0, 0);
        if (!(v20 & 1))
        {
            do
            {
                *((st_4d9d31_0 **)(esp - 4)) = v7;
                v23 = _ccall(v17, v19, v18, 0);
                v24 = -(v23 & 1);
                v25 = _ccall(12, 0, v23 & 1, v23 & 1);
                v26 = _ccall(CONCAT((unsigned short)v25, *((unsigned short *)&v10)), 63);
                v27 = v26;
                x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_23{r16|4b}, 16bits@0<64>)), Conv(8->32, vvar_85{r8|1b}), 1<32>)
                *((char *)iter - 44) = *((char *)iter - 44) - v27 - ((char)(v26 >> 16) & 213 & 1);
                iter += 0xfffffffc;
                v17 = 3;
                v19 = v21;
                v18 = v28;
                esp = &g_e95eb278;
                g_e95eb278 = iter;
                v10 = v21 + v28;
                v30 = x86g_dirtyhelper_IN(3<32>, 4<32>);
                if (!(v21 + v28))
                {
                    InterlockedExchange8(v5 - 1160096670, *((char *)&v10));
                    syscall();
                    g_e95eb275 = v6;
                    v31 = _ccall(0, 13, (char)v30 & *(v11), 0, 0);
                    if (!(v31 & 1))
                    {
                        v32 = _ccall(14, 13, (char)v30 & *(v11), 0, 0);
                        if (v32 & 1)
                            goto LABEL_0x4d9dfa;
                        else
                            goto LABEL_0x4d9e36;
                    }
                }
            } while ((v4 = v30, !((v19 + v18 >= 0x80000000 ? 1 : 0) & 1)));
        }
        *(v24) = *((char *)&v10);
    }
}
