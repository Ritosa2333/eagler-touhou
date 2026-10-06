// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484b1d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_484b1d_1 {
    char padding_0[112];
    unsigned int field_70;
} st_484b1d_1;

int sub_484b1d(void)
{
    unsigned int v10;  // edi
    unsigned int v11;  // edi
    unsigned int v20;  // eax
    void* idx;  // ebx
    void* v22;  // edi
    unsigned int v23;  // edi
    unsigned int v24;  // eax
    unsigned int v25;  // edi
    unsigned int v26;  // edi
    unsigned int v27;  // edi
    unsigned int v28;  // edi
    int v29;  // eax
    unsigned int v12;  // edx
    void* v13;  // esi
    unsigned int v14;  // ebx
    unsigned int v15;  // ebx
    void* v16;  // eax
    void* v17;  // eax
    void* v18;  // eax
    void* v19;  // eax
    unsigned int v0;  // [bp-0x34]
    st_46d3b4_0 v1;  // [bp-0x28]
    st_484b1d_1 *index;  // [bp-0x20]
    char v3;  // [bp-0x1c]
    void* v4;  // [bp-0x18], Other Possible Types: unsigned int
    void* iter1;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int node;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    void* iter;  // [bp-0x8], Other Possible Types: unsigned int
    unsigned int *v9;  // [bp+0x4]

    v0 = v10;
    v11 = 0;
    sub_46d3b4(&v1, v12, v9);
    v13 = *((int *)(v1 + 212));
    iter = 0;
    do
    {
        v14 = iter * 4;
        v4 = sub_475860(*((int *)(v14 + (char *)v13 + 28)));
        iter += 1;
        v11 = sub_475860(*((int *)(v14 + (char *)v13))) + v11 + v4 + 2;
    } while (iter < 7);
    iter1 = v13 + 56;
    node = 12;
    do
    {
        v15 = iter1;
        v4 = sub_475860(*((int *)(v15 + 48)));
        iter1 += 4;
        node -= 1;
        v11 = sub_475860(*((int *)v15)) + v11 + v4 + 2;
    } while (node != 1);
    v16 = sub_475860((int)v13[156]);
    v17 = sub_475860((int)v13[152]);
    v18 = sub_475860((int)v13[160]);
    v19 = sub_475860((int)v13[164]);
    v20 = v17 + v11 + v16 + 2 + v18 + 1 + v19 + 1 + sub_475860((int)v13[168]) + 185;
    v7 = v20;
    idx = sub_4777ce(v7);
    if (idx)
    {
        v22 = idx + 184;
        sub_47a330(idx, v13, 184);
        iter = 0;
        iter1 = idx;
        iter1 -= v13;
        node = v13 + 28;
        do
        {
            *((void* *)((char *)idx + 4 * iter)) = v22;
            if (sub_47a2b9(v22, idx - v22 + v7, *((int *)(node - 28))))
                sub_470dc6(0, 0, 0, 0, 0);
            v23 = v22 + sub_475860(v22) + 1;
            *((unsigned int *)(iter1 + node)) = v23;
            if (sub_47a2b9(v23, idx - v23 + v7, *((int *)node)))
                sub_470dc6(0, 0, 0, 0, 0);
            iter += 1;
            node += 4;
            v22 = v23 + sub_475860(v23) + 1;
        } while (iter < 7);
        iter = idx + 104;
        v24 = v13 + 56;
        node = v24;
        v4 = 12;
        while (1)
        {
            *((void* *)(v24 + iter1)) = v22;
            if (sub_47a2b9(v22, idx - v22 + v7, *((int *)v24)))
                sub_470dc6(0, 0, 0, 0, 0);
            v25 = v22 + sub_475860(v22) + 1;
            *((unsigned int *)iter) = v25;
            if (sub_47a2b9(v25, idx - v25 + v7, *((int *)(node + 48))))
                sub_470dc6(0, 0, 0, 0, 0);
            node += 4;
            iter += 4;
            v4 -= 1;
            v22 = v25 + sub_475860(v25) + 1;
            if (v4 == 1)
                break;
            v24 = node;
        }
        *((void* *)&idx[152]) = v22;
        if (sub_47a2b9(v22, idx - v22 + v7, (int)v13[152]))
            sub_470dc6(0, 0, 0, 0, 0);
        v26 = v22 + sub_475860(v22) + 1;
        *((unsigned int *)&idx[156]) = v26;
        if (sub_47a2b9(v26, idx - v26 + v7, (int)v13[156]))
            sub_470dc6(0, 0, 0, 0, 0);
        v27 = v26 + sub_475860(v26) + 1;
        *((unsigned int *)&idx[160]) = v27;
        if (sub_47a2b9(v27, idx - v27 + v7, (int)v13[160]))
            sub_470dc6(0, 0, 0, 0, 0);
        v28 = v27 + sub_475860(v27) + 1;
        *((unsigned int *)&idx[164]) = v28;
        if (sub_47a2b9(v28, idx - v28 + v7, (int)v13[164]))
            sub_470dc6(0, 0, 0, 0, 0);
        v29 = v28 + sub_475860(v28) + 1;
        *((int *)&idx[168]) = v29;
        if (sub_47a2b9(v29, idx - v29 + v7, (int)v13[168]))
            sub_470dc6(0, 0, 0, 0, 0);
    }
    if (v3)
        index->field_70 = index->field_70 & 0xfffffffd;
    return;
}
