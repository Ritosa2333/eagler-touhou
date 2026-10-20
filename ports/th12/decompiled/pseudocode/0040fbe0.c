// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40fbe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40fbe0_1 {
    char padding_0[4];
    struct st_40fbe0_2 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
} st_40fbe0_1;

typedef struct st_40fbe0_2 {
    unsigned int field_0;
} st_40fbe0_2;

typedef struct st_40fbe0_0 {
    char padding_0[16];
    int field_10;
} st_40fbe0_0;

extern unsigned int g_4ad138;
extern st_40fbe0_0 *g_4b43d4;

int sub_40fbe0(unsigned int a0, unsigned int a1)
{
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned int *index;  // esi
    unsigned int *v17;  // eax
    unsigned int *idx;  // ebx
    unsigned int v19;  // eax
    unsigned long long v20;  // 4123
    unsigned short v8;  // fs
    unsigned long long v9;  // 4143
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned long long v12;  // 4165
    unsigned int idx1;  // edi
    unsigned short *v14;  // ax
    st_40fbe0_1 *idx2;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int *v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v5 = 0xffffffff;
    v4 = sub_49750b;
    v9 = _ccall(v6, v7, v8, 0);
    v3 = *((int *)(unsigned int)v9);
    v2 = v10;
    v1 = v11;
    v0 = g_4ad138 ^ &edi;
    v12 = _ccall(v6, v7, v8, 0);
    *((unsigned int **)(unsigned int)v12) = &v3;
    idx1 = a1 * 3;
    v14 = (unsigned short *)*((short *)(8 * idx1 + 4911456));
    idx2 = 8 * idx1 + 4911456;
    *((unsigned int **)a0) = NULL;
    if (v14 >= NULL)
    {
        sub_4615a0(g_4b43d4->field_10, &a1, v14, 0);
        *((unsigned int **)a0) = v4;
        index = sub_461920(v4);
        if (!index)
            *((unsigned int **)a0) = index;
        if (idx2->field_4)
            idx2->field_4();
        index[290] = idx2->field_8;
        index[291] = idx2->field_c;
        index[292] = idx2->field_10;
        index[293] = idx2->field_14;
    }
    else if (v14 == 0xffff)
    {
        v17 = sub_46c9ea(32);
        if (v17)
        {
            memset(v17, 0, 32);
            idx = v17;
        }
        else
        {
            idx = NULL;
        }
        sub_4109f0();
        a1 = sub_46c9ea(24);
        v19 = 0;
        v5 = 0;
        if (a1)
            v19 = sub_40ff10(16);
        idx[5] = v19;
        idx[6] = idx2->field_8;
        idx[7] = idx2->field_c;
    }
    v20 = _ccall(v6, v7, v8, 0);
    *((unsigned int *)(unsigned int)v20) = v0;
    return a0;
}
