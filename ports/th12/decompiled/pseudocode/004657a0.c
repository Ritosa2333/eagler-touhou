// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4657a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4657a0_3 {
    unsigned int field_0;
    char padding_4[40];
    unsigned int field_2c;
    char padding_30[72];
    unsigned int field_78;
    unsigned int field_7c;
    char padding_80[12];
    HANDLE field_8c;
    unsigned int field_90;
} st_4657a0_3;

typedef struct st_4657a0_0 {
    char padding_0[12];
    struct st_4657a0_1 *field_c;
} st_4657a0_0;

typedef struct st_4657a0_2 {
    struct st_4657a0_0 *field_0;
} st_4657a0_2;

typedef struct st_4657a0_1 {
    unsigned int field_0;
} st_4657a0_1;

typedef struct st_4657a0_7 {
    char padding_0[8];
    struct st_4657a0_1 *field_8;
} st_4657a0_7;

typedef struct st_4657a0_4 {
    char padding_0[56];
    unsigned int field_38;
    char padding_3c[32];
    unsigned int field_5c;
    char padding_60[20];
    unsigned int field_74;
    unsigned int field_78;
} st_4657a0_4;

extern unsigned int g_49981c;
extern unsigned int g_4ad138;

int sub_4657a0(st_4657a0_2 **a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, int a5, unsigned int a6, unsigned int a7)
{
    unsigned long v22;  // ldt
    unsigned long v23;  // gdt
    int v32;  // edi
    unsigned int idx;  // eax
    int v34;  // ecx
    st_4657a0_7 **v35;  // eax
    unsigned int v36;  // edx
    unsigned int *v37;  // eax
    st_4657a0_4 **v38;  // edx
    unsigned int *iter;  // edi
    unsigned int v40;  // ecx
    unsigned int *j;  // esi
    unsigned short v24;  // fs
    unsigned long long v42;  // 4122
    unsigned int v43;  // eax
    unsigned long long v25;  // 4234
    unsigned int v26;  // ebx
    unsigned long long v27;  // 4238
    unsigned int v28;  // ecx
    st_4657a0_3 *index;  // eax
    st_4657a0_3 *idx1;  // ebp
    st_4657a0_2 **v31;  // esi
    unsigned int v0;  // [bp-0x84]
    unsigned long v1;  // [bp-0x78]
    char *v2;  // [bp-0x74]
    unsigned int v3;  // [bp-0x6c]
    unsigned int v4;  // [bp-0x5c]
    unsigned int v5;  // [bp-0x58]
    int <0x4657a0[is_4]|Stack bp-0x58, 1 B>;  // [bp-0x58]
    unsigned int v6;  // [bp-0x54]
    unsigned int v7;  // [bp-0x4c]
    unsigned int v8;  // [bp-0x44]
    unsigned int v9;  // [bp-0x40]
    unsigned int v10;  // [bp-0x3c]
    unsigned int v11;  // [bp-0x38]
    unsigned int v12;  // [bp-0x34]
    unsigned int v13;  // [bp-0x30]
    unsigned int v14;  // [bp-0x2c]
    unsigned int v15;  // [bp-0x28]
    unsigned int v16;  // [bp-0x24]
    unsigned int v17;  // [bp-0x10]
    unsigned int v18;  // [bp-0xc]
    unsigned int v19;  // [bp-0x8]
    unsigned int v20;  // [bp-0x4]
    unsigned int v21;  // [bp+0x0]

    v20 = 0xffffffff;
    v19 = sub_496e4b;
    v25 = _ccall(v22, v23, v24, 0);
    v18 = *((int *)(unsigned int)v25);
    v17 = g_4ad138 ^ &<0x4657a0[is_4]|Stack bp-0x58, 1 B>;
    v4 = v26;
    v3 = g_4ad138 ^ &edi;
    v27 = _ccall(v22, v23, v24, 0);
    *((unsigned int **)(unsigned int)v27) = &v18;
    v7 = v28;
    if (*(a0))
    {
        v6 = 0;
        v5 = 0;
        index = sub_46c9ea(156);
        if (index)
        {
            index->field_90 = 0;
            index->field_0 = 0;
            index->field_2c = 0;
            index->field_7c = 0;
            idx1 = index;
        }
        else
        {
            idx1 = NULL;
        }
        idx1->field_7c = 0;
        idx1->field_78 = 1;
        if (sub_466b10())
        {
            if (idx1->field_78 == 1)
            {
                CloseHandle(idx1->field_8c);
                idx1->field_8c = 0xffffffff;
            }
            sub_46ca4f(idx1);
LABEL_465885:
        }
        else
        {
            v8 = 0;
            v9 = 0;
            v10 = 0;
            v12 = 0;
            v13 = 0;
            v14 = 0;
            v15 = 0;
            v16 = 0;
            v11 = 0;
            v13 = a1;
            v15 = a3;
            v16 = a4;
            v14 = a2;
            v8 = 36;
            v9 = 98696;
            v10 = a5 * 16;
            v2 = &v6;
            v12 = idx1->field_90 + 32;
            v1 = &v8;
            if (*(a0)->field_0->field_c(*(a0), v1, *((unsigned int *)((void*)&v1 + 4)), 0) < 0 || (v0 = (unsigned int)&g_49981c, *(v31)->field_0(v31, &g_49981c, &edi) < 0))
                goto LABEL_465885;
            v32 = sub_46c9ea(-0x1);
            if (v32)
            {
                idx = 0;
                v34 = a5 - 1;
                do
                {
                    *((int *)(v32 + idx * 8)) = v34;
                    *((unsigned int *)(v32 + idx * 8 + 4)) = v21;
                    idx += 1;
                    v34 += a5;
                } while (idx < 16);
                v35 = &edi;
                if ((*((int *)(*((int *)*((unsigned int *)((void*)&v1 + 4))) + 12)))(*((unsigned int *)((void*)&v1 + 4)), 16, v32) < NULL)
                {
                    if (v35)
                    {
                        *(v35)->field_8(v35);
                        v0 = 0;
                    }
                    sub_46ca4f(v32);
                }
                else
                {
                    if (v35)
                    {
                        *(v35)->field_8(v35);
                        v0 = 0;
                    }
                    sub_46ca4f(v32);
                    v36 = sub_46c9ea(124);
                    (&v1)[1] = v36;
                    v37 = NULL;
                    v13 = 0;
                    if (v36)
                        v37 = sub_466650(&edi, a5);
                    v38 = &edi;
                    *(v38) = v37;
                    iter = v37 + 14;
                    v40 = 9;
                    for (j = &v1; v40; j += 1)
                    {
                        v40 -= 1;
                        *(iter) = *(j);
                        iter += 1;
                    }
                    *(v38)->field_5c = v0;
                    *(v38)->field_74 = a3;
                    *(v38)->field_78 = 0;
                }
            }
        }
    }
    v42 = _ccall(v22, v23, v24, 0);
    *((unsigned int *)(unsigned int)v42) = a2;
    _security_check_cookie();
    return v43;
}
