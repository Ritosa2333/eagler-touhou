// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465aa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465aa0_0 {
    char padding_0[12];
    struct st_465aa0_1 *field_c;
} st_465aa0_0;

typedef struct st_465aa0_2 {
    struct st_465aa0_0 *field_0;
} st_465aa0_2;

typedef struct st_465aa0_1 {
    unsigned int field_0;
} st_465aa0_1;

typedef struct st_465aa0_6 {
    char padding_0[8];
    struct st_465aa0_1 *field_8;
} st_465aa0_6;

typedef struct st_465aa0_3 {
    char padding_0[56];
    unsigned int field_38;
    char padding_3c[32];
    unsigned int field_5c;
    char padding_60[20];
    unsigned int field_74;
    unsigned int field_78;
} st_465aa0_3;

extern unsigned int g_49981c;
extern unsigned int g_4ad138;

int sub_465aa0(unsigned int a0, unsigned int a1, st_465aa0_2 **a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int a8, int a9)
{
    unsigned long v20;  // ldt
    unsigned long v21;  // gdt
    unsigned int idx;  // eax
    int v31;  // ecx
    st_465aa0_6 **v32;  // eax
    unsigned int v33;  // edx
    unsigned int *v35;  // eax
    st_465aa0_3 **v36;  // edx
    unsigned int *iter;  // edi
    unsigned int v38;  // ecx
    unsigned int *j;  // esi
    unsigned short v22;  // fs
    unsigned long long v40;  // 4122
    unsigned int v41;  // eax
    unsigned long long v23;  // 4231
    unsigned int v24;  // ebx
    unsigned long long v25;  // 4235
    unsigned int *idx1;  // esi
    unsigned int *index;  // eax
    st_465aa0_2 **v28;  // esi
    int v29;  // ebx
    unsigned int v0;  // [bp-0x84]
    unsigned long v1;  // [bp-0x78]
    unsigned int v2;  // [bp-0x6c]
    unsigned int v3;  // [bp-0x5c]
    unsigned int v4;  // [bp-0x58]
    int <0x465aa0[is_4]|Stack bp-0x58, 1 B>;  // [bp-0x58]
    unsigned int v5;  // [bp-0x54]
    unsigned int v6;  // [bp-0x4c]
    unsigned int v7;  // [bp-0x44]
    unsigned int v8;  // [bp-0x40]
    unsigned int v9;  // [bp-0x3c]
    unsigned int v10;  // [bp-0x38]
    unsigned int v11;  // [bp-0x34]
    unsigned int v12;  // [bp-0x30]
    unsigned int v13;  // [bp-0x2c]
    unsigned int v14;  // [bp-0x28]
    unsigned int v15;  // [bp-0x24]
    unsigned int v16;  // [bp-0x10]
    unsigned int v17;  // [bp-0xc]
    unsigned int v18;  // [bp-0x8]
    unsigned int v19;  // [bp-0x4]

    v19 = 0xffffffff;
    v18 = sub_496e0b;
    v23 = _ccall(v20, v21, v22, 0);
    v17 = *((int *)(unsigned int)v23);
    v16 = g_4ad138 ^ &<0x465aa0[is_4]|Stack bp-0x58, 1 B>;
    v3 = v24;
    v2 = g_4ad138 ^ &edi;
    v25 = _ccall(v20, v21, v22, 0);
    *((unsigned int **)(unsigned int)v25) = &v17;
    v6 = a0;
    idx1 = NULL;
    if (*(a2))
    {
        v5 = 0;
        v4 = 0;
        index = sub_46c9ea(156);
        if (index)
        {
            index[36] = 0;
            *(index) = 0;
            index[11] = 0;
            index[31] = 0;
            idx1 = index;
        }
        idx1[36] = a4;
        idx1[32] = a1;
        idx1[33] = a1;
        idx1[0x22] = a3;
        idx1[31] = 1;
        v7 = 0;
        v8 = 0;
        v9 = 0;
        v11 = 0;
        v12 = 0;
        v13 = 0;
        v14 = 0;
        v15 = 0;
        v10 = 0;
        v13 = a6;
        v14 = a7;
        v7 = 36;
        v8 = 98696;
        v9 = a9 * 16;
        v12 = a5;
        v15 = a8;
        v11 = idx1[36] + 32;
        v1 = &v7;
        if (*(a2)->field_0->field_c(*(a2), v1, *((unsigned int *)((void*)&v1 + 4)), 0) >= NULL && !(v0 = (unsigned int)&g_49981c, *(v28)->field_0(v28, &g_49981c, &edi) < 0))
        {
            v29 = sub_46c9ea(-0x1);
            if (v29)
            {
                idx = 0;
                v31 = a9 - 1;
                do
                {
                    *((int *)(v29 + idx * 8)) = v31;
                    *((unsigned int *)(v29 + idx * 8 + 4)) = a3;
                    idx += 1;
                    v31 += a9;
                } while (idx < 16);
                v32 = &edi;
                if ((*((int *)(*((int *)*((unsigned int *)((void*)&v1 + 4))) + 12)))(*((unsigned int *)((void*)&v1 + 4)), 16, v29) < NULL)
                {
                    if (v32)
                    {
                        *(v32)->field_8(v32);
                        v0 = 0;
                    }
                    sub_46ca4f(v29);
                    goto LABEL_465c9a;
                }
                else
                {
                    if (v32)
                    {
                        *(v32)->field_8(v32);
                        v0 = 0;
                    }
                    sub_46ca4f(v29);
                    v33 = sub_46c9ea(124);
                    (&v1)[1] = v33;
                    v12 = 0;
                    v35 = (!v33 ? NULL : sub_466650(&edi, a9));
                    v36 = &edi;
                    *(v36) = v35;
                    iter = v35 + 14;
                    v38 = 9;
                    for (j = &v1; v38; j += 1)
                    {
                        v38 -= 1;
                        *(iter) = *(j);
                        iter += 1;
                    }
                    *(v36)->field_5c = v0;
                    *(v36)->field_74 = v16;
                    *(v36)->field_78 = 0;
                }
            }
        }
        else
        {
LABEL_465c9a:
        }
    }
    v40 = _ccall(v20, v21, v22, 0);
    *((unsigned int *)(unsigned int)v40) = v14;
    _security_check_cookie();
    return v41;
}
