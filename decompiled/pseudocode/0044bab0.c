// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bab0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bab0_0 {
    char padding_0[8];
    unsigned int field_8;
} st_44bab0_0;

extern unsigned int g_4ad138;

void* sub_44bab0(unsigned int a0, unsigned long long a1)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    int v15;  // ebp
    void* v16;  // edi
    void* v17;  // esi
    void* iter;  // ebp
    void* v19;  // eax
    void* v20;  // eax
    void* v21;  // eax
    void* v22;  // eax
    unsigned int v23;  // eax
    void* node;  // ecx
    unsigned short v7;  // fs
    char j;  // 4102
    void* v26;  // eax
    void* v27;  // eax
    void* v28;  // eax
    void* v29;  // eax
    unsigned int v30;  // eax
    unsigned int v31;  // ecx
    unsigned int v32;  // ecx
    unsigned int v33;  // ecx
    unsigned int v34;  // 4101
    unsigned long long v8;  // 4170
    void* v35;  // esi
    void* v36;  // esi
    void* v37;  // esi
    st_44bab0_0 *idx;  // eax
    unsigned long long v39;  // 4155
    unsigned long long v40;  // 4123
    unsigned long long v9;  // 4190
    unsigned long long v10;  // ebp
    unsigned long long v11;  // eax
    unsigned int v12;  // ecx
    int v13;  // edi
    int v14;  // esi
    void* v0;  // [bp-0x14]
    void* iter1;  // [bp-0x10], Other Possible Types: unsigned long long
    unsigned long long v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = 0xffffffff;
    v3 = sub_496edb;
    v8 = _ccall(v5, v6, v7, 0);
    v2 = *((int *)(unsigned int)v8);
    v9 = _ccall(v5, v6, v7, 0);
    *((unsigned long long **)(unsigned int)v9) = &v2;
    v10 = a1;
    v11 = v10 + 1;
    v12 = -((v11 >= 0x8000000 ? 1 : 0) & 1) | (unsigned int)(v11 * 16);
    iter1 = sub_46c9ea(-(v12 + 4 < 0) | v12 + 4, g_4ad138 ^ &v13, v13, v14, v15);
    v4 = 0;
    if (iter1)
    {
        v16 = iter1 + 4;
        *((unsigned int *)iter1) = v11;
        v0 = v16;
        sub_46ce49(v16, 16, v11, sub_44bcc0, sub_44bc50);
        if (v16)
        {
            v17 = iter1;
            if (v10 > NULL)
            {
                iter = v16 + 8;
                iter1 = v2;
                do
                {
                    v19 = v17;
                    v20 = v19;
                    do
                    {
                        v21 = v20;
                        v22 = v21 + 1;
                        v20 = v22;
                    } while (*((char *)v21));
                    v23 = sub_46d04a(v22 - (v19 + 1) + 1);
                    if (v23)
                    {
                        node = v17;
                        do
                        {
                            j = *((char *)node);
                            *((char *)(v23 - v17 + node)) = *((char *)node);
                            node += 1;
                        } while (j);
                        v16 = sub_44bc50;
                    }
                    *((unsigned int *)((char *)iter - 8)) = v23;
                    v26 = v17;
                    v27 = v26;
                    do
                    {
                        v28 = v27;
                        v29 = v28 + 1;
                        v27 = v29;
                    } while (*((char *)v28));
                    v30 = v29 - (v26 + 1) + 1;
                    v31 = v30;
                    v32 = v31 & 0x80000003;
                    if ((v31 & 0x80000003) < NULL)
                    {
                        v33 = v32 - 1 | 0xfffffffc;
                        v32 = v33 + 1;
                        v34 = _ccall(4, 18, v33 + 1, 0, 0);
                        if (v34 & 1)
                            continue;
                    }
                    else if (!(v31 & 0x80000003))
                    {
                        continue;
                    }
                    v30 += 4 - v32;
                    v35 = v17 + v30;
                    v36 = v35 + 4;
                    *((int *)((char *)iter - 4)) = *((int *)v35);
                    v37 = v36 + 4;
                    *((int *)iter) = *((int *)v36);
                    *((int *)&iter[4]) = *((int *)v37);
                    v17 = v37 + 4;
                    iter += 16;
                    iter1 -= 1;
                } while (iter1 != 1);
                v10 = v2;
            }
            idx = v16 + v10 * 16;
            *((unsigned int *)&idx->padding_0[4]) = v3;
            idx->field_8 = 0;
            v39 = _ccall(v5, v6, v7, 0);
            *((int *)(unsigned int)v39) = v13;
            return v16;
        }
    }
    v40 = _ccall(v5, v6, v7, 0);
    *((int *)(unsigned int)v40) = v13;
    return NULL;
}
