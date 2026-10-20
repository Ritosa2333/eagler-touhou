// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbf59; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dbf59_1 {
    unsigned int field_0;
} st_4dbf59_1;

typedef struct st_4dbf59_2 {
    unsigned int field_0;
    int field_4;
} st_4dbf59_2;

typedef struct st_4dbf59_0 {
    char padding_0[94];
    struct st_4dbf59_1 *field_5e;
    char padding_62[141];
    char field_ef;
    char padding_f0[84];
    void* field_144;
    int field_148;
    char padding_14c[828];
    unsigned int field_488;
} st_4dbf59_0;


int sub_4dbf59(void)
{
    st_4dbf59_2 *v5;  // esi
    st_4dbf59_0 *idx;  // ebp
    int v15;  // ecx
    unsigned int v16;  // 4101
    void* v17;  // esi
    void* iter;  // edi
    void* node;  // esi
    int v21;  // ecx
    unsigned int v22;  // ecx
    int v7;  // ebx
    int v8;  // eax
    int v9;  // eax
    unsigned int v10;  // ebx
    unsigned int v11;  // ecx
    int v12;  // ecx
    void* v13;  // esi
    unsigned int iter1;  // ebx
    unsigned int v0;  // [bp-0x24]
    st_4dbf59_2 *v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    int v3;  // [bp-0x18]

    v7 = v5->field_0 + *((int *)&idx->padding_14c[824]);
    v9 = sub_4dc539(v7, v8, v5->field_4, idx->field_144, v5);
    v10 = _INSERT(v7, 0, 0);
    idx->padding_62[139] = idx->padding_62[139] + 1;
    v3 = v9;
    v2 = v11;
    v0 = v10;
    v12 = v9 - 6;
    v13 = *((int *)&idx->padding_f0[82]);
    iter1 = 0;
    while (1)
    {
        v15 = v12;
        if (!v12 || !(v16 = (unsigned int)_ccall(8, 15, v12, 0, 0), !(v16 & 1)))
            break;
        v17 = v13 + 1;
        if (*((char *)v13) != 232 && *((char *)v13) != 233 || *((char *)v17))
        {
            iter1 += 1;
            v12 = v15 - 1;
            v13 = v17;
        }
        else
        {
            *((unsigned int *)v17) = __ROL__(_INSERT(*((int *)v17), 0, 0), 24) - iter1;
            iter1 += 5;
            v13 = v17 + 4;
            v12 = v15 - 5;
        }
    }
    iter = v1->field_0 + *((int *)&idx->padding_14c[824]);
    node = *((int *)&idx->padding_f0[82]);
    for (v21 = v9 >> 2; v21; node += 4)
    {
        v21 -= 1;
        *((int *)iter) = *((int *)node);
        iter += 4;
    }
    for (v22 = v9 & 3; v22; node += 1)
    {
        v22 -= 1;
        *((char *)iter) = *((char *)node);
        iter += 1;
    }
    (*((int *)&(idx->padding_0)[1]))(*((int *)&idx->padding_f0[82]), 0, 0x8000);
}
