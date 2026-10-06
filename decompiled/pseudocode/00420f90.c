// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x420f90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_420f90_0 {
    char padding_0[27712];
    unsigned int field_6c40;
    char padding_6c44[28];
    unsigned int field_6c60;
    unsigned int field_6c64;
    int field_6c68;
    char padding_6c6c[72];
    int field_6cb4;
    unsigned int field_6cb8;
    char padding_6cbc[136];
    unsigned int field_6d44;
} st_420f90_0;

typedef struct st_420f90_2 {
    char padding_0[27752];
    int field_6c68;
    char padding_6c6c[72];
    unsigned int field_6cb4;
    unsigned int field_6cb8;
    char padding_6cbc[136];
    int field_6d44;
} st_420f90_2;

typedef struct st_420f90_1 {
    char padding_0[102324];
    int field_18fb4;
} st_420f90_1;

extern unsigned int g_421334[4];
extern st_420f90_1 *g_4b43b8;
extern unsigned int g_4ce8cc;

int sub_420f90(void)
{
    unsigned int v14;  // ebx
    unsigned int v15;  // edi
    unsigned int v24;  // eax
    unsigned int v25;  // ecx
    st_420f90_2 *index;  // esi
    int v27;  // edx
    int *idx;  // esi
    int *idx1;  // esi
    int *idx2;  // esi
    int *v31;  // esi
    unsigned int v16;  // eax
    st_420f90_0 *v17;  // esi
    int v18;  // ebp
    unsigned int v19;  // ebx
    st_420f90_0 *iter;  // edi
    unsigned int v21;  // eax
    int v22;  // ecx
    unsigned int v23;  // ecx
    unsigned int v0;  // [bp-0x60]
    unsigned int v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x48]
    unsigned int v4;  // [bp-0x44]
    unsigned int v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x34]
    unsigned int v8;  // [bp-0x24]
    unsigned int v9;  // [bp-0x20]
    unsigned int v10;  // [bp-0x18]
    char v11;  // [bp-0x10], Other Possible Types: unsigned int
    char v12;  // [bp+0x0]

    v10 = v14;
    v9 = v15;
    goto *((void *)(g_421334[v16]));

    switch (/* incomplete */)
    {
    case 0x421329:
LABEL_421329:
        return;
    case 0x421263:
        sub_461a70(idx1[0x1b1b]);
        idx1[0x1b1b] = 0;
        sub_4615a0(idx1[6993], &v12, 46, 0);
        idx1[0x1b1b] = (int)v11;
        return;
    case 0x4212a8:
        sub_461a70(idx2[0x1b1b]);
        idx2[0x1b1b] = 0;
        sub_4615a0(idx2[6993], &v12, 47, 0);
        idx2[0x1b1b] = (int)v11;
        return;
    case 0x4211a9:
        sub_461a70(index->field_6c68);
        index->field_6c68 = 0;
        sub_4615a0(index->field_6d44, &v12, 44, 0);
        v27 = index->field_6d44;
        index->field_6c68 = (int)v11;
        index->field_6cb8 = 1;
        sub_4615a0(v27, &v11, 78, 0);
        index->field_6cb4 = v9;
        return;
    case 0x4212ed:
        sub_461a70(v31[6938]);
        v31[6938] = 0;
        sub_4615a0(v31[6993], &v12, 48, 0);
        v31[6938] = (int)v11;
        goto LABEL_421329;
    case 0x42121e:
        sub_461a70(idx[0x1b1b]);
        idx[0x1b1b] = 0;
        sub_4615a0(idx[6993], &v12, 45, 0);
        idx[0x1b1b] = (int)v11;
        return;
    case 0x420f9d:
        sub_461a70(v17->field_6c68);
        v18 = 0;
        v17->field_6c68 = 0;
        v7 = v17->field_6d44;
        sub_4615a0(v17->field_6d44, &v11, 43, 0);
        v17->field_6c68 = (int)v9;
        v9 = v11;
        v19 = 10000000;
        v8 = 0;
        iter = v17 + 27712;
        do
        {
            v2 = (unsigned int)*((int *)&iter->padding_0[0]);
            sub_461a70(*((int *)&iter->padding_0[0]));
            v1 = 0;
            *((unsigned int *)&iter->padding_0[0]) = 0;
            sub_4615a0(g_4b43b8->field_18fb4, &v7, v18 + 5, 0);
            v21 = v3;
            *((unsigned int *)&iter->padding_0[0]) = v4;
            v5 = v21 % v19;
            v3 = v21 / v19;
            if (v3)
                v2 = 1;
            if (sub_461920(v4, g_4ce8cc, v4))
                sub_454b80();
            if (!v2)
                sub_461d80();
            else
                sub_461d40();
            v3 = v21 % v19;
            v18 = (int)(v18 + 1);
            iter += 4;
            v19 = (unsigned int)(((unsigned int)((int)(1717986919 * v19 >> 0x22)) >> 31) + (int)(1717986919 * v19 >> 0x22));
        } while (v18 < 8);
        v0 = v17->field_6c60;
        sub_461a70(v17->field_6c60);
        v17->field_6c60 = 0;
        if (v6 >= 1000000)
        {
            v22 = g_4b43b8->field_18fb4;
            sub_4615a0(v22, &v6, 13, 0);
            v17->field_6c60 = v2;
            if (sub_461920(v23, g_4ce8cc, v2))
                sub_454b80();
        }
        sub_461a70(v17->field_6c64);
        v17->field_6c64 = 0;
        if (v6 >= 1000)
        {
            sub_4615a0(g_4b43b8->field_18fb4, &v1, 14, 0);
            v24 = v17->field_6c60;
            v17->field_6c64 = v24;
            if (sub_461920(v23, g_4ce8cc, v24))
                sub_454b80();
        }
        v25 = v17->field_6d44;
        v17->field_6cb8 = 1;
        sub_4615a0(v25, &v0, 78, 0);
        v17->field_6cb4 = v22;
        return;
    }
}
