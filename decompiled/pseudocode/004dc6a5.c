// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc6a5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc6a5_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[56];
    unsigned int field_44;
    unsigned int field_48;
    char padding_4c[56];
    unsigned int field_84;
    unsigned int field_88;
    unsigned int field_8c;
} st_4dc6a5_0;

typedef struct st_4dc6a5_1 {
    char field_0;
    char padding_1[3];
    unsigned int field_4;
} st_4dc6a5_1;


char sub_4dc6a5(st_4dc6a5_0 *a0, unsigned int a1, unsigned int a2)
{
    st_4dc6a5_0 *idx;  // edx
    unsigned int v10;  // ecx
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // ebx
    unsigned int v22;  // esi
    char v23;  // al
    unsigned int v24;  // ecx
    void* node;  // edi
    unsigned int v27;  // ebx
    unsigned int v28;  // eax
    unsigned int v11;  // ebp
    unsigned int v29;  // ecx
    unsigned int v30;  // ecx
    unsigned int v31;  // ecx
    unsigned int idx1;  // eax
    unsigned int j;  // eax
    st_4dc6a5_1 *iter;  // edi
    unsigned int v14;  // esi
    unsigned int idx2;  // ecx
    int k;  // ecx
    unsigned int v17;  // edi
    st_4dc6a5_0 *iter1;  // ebp
    unsigned int v0;  // [bp-0x98]
    unsigned int v1;  // [bp-0x94]
    int v2;  // [bp-0x90]
    unsigned int v3;  // [bp-0x8c]
    st_4dc6a5_0 *v4;  // [bp-0x88]
    unsigned int v5;  // [bp-0x84]
    int <0x4dc6a5[is_1]|Stack bp-0x80, 1 B>;  // [bp-0x80]
    int v6;  // [bp-0x80]
    unsigned int v8;  // [bp-0x40]
    unsigned long v33;  // [bp-0x3c]

    idx = a0;
    v10 = 15;
    v11 = idx->field_84;
    j = 0;
    iter = &<0x4dc6a5[is_1]|Stack bp-0x80, 1 B> + 1;
    for (v14 = 0; v10; iter = &iter->field_4)
    {
        v10 -= 1;
        *((unsigned int *)&iter->field_0) = 0;
    }
    v4 = idx;
    if (v11 > 0)
    {
        do
        {
            idx2 = *((char *)(j + a2));
            j += 1;
            (&<0x4dc6a5[is_1]|Stack bp-0x80, 1 B>)[idx2] = (&<0x4dc6a5[is_1]|Stack bp-0x80, 1 B>)[idx2] + 1;
        } while (j < v11);
    }
    k = 23;
    v6 = (int)_INSERT(<0x4dc6a5[is_1]|Stack bp-0x80, 1 B>, 0, 0);
    idx->field_4 = 0;
    idx->field_44 = 0;
    v8 = 0;
    v17 = 0;
    v3 = 0;
    v0 = 1;
    v2 = 23;
    iter1 = &idx->field_8;
    v1 = 0;
    do
    {
        v17 += *((int *)((char *)&v6 + v14 + 4)) << ((char)k & 31);
        v5 = v17;
        if (v17 > 0x1000000)
            return 0;
        v19 = *((int *)((char *)&v6 + v14));
        *((unsigned int *)&iter1->padding_0[0]) = v17;
        v20 = v19 + *((int *)&iter1->padding_c[48]);
        *((unsigned int *)&iter1->padding_c[52]) = v20;
        *((unsigned int *)((char *)&v33 + v14)) = v20;
        if (k >= 16)
        {
            v21 = v3;
            v22 = (unsigned int)(iter1->padding_0 >> 16);
            v23 = v0;
            v24 = v22 - v21;
            node = idx->field_8c + v21;
            v27 = _INSERT(_INSERT(v21, 0, v23), 1, v23);
            v3 = v22;
            v14 = v1;
            v28 = _INSERT(v27 * 0x10000, 0, v27);
            for (v29 = v24 >> 2; v29; node += 4)
            {
                v29 -= 1;
                *((unsigned int *)node) = v28;
            }
            idx = v4;
            for (v30 = v24 & 3; v30; node += 1)
            {
                v30 -= 1;
                *((char *)node) = v28;
            }
            v17 = v5;
            k = v2;
        }
        v14 += 4;
        k -= 1;
        iter1 = &iter1->field_4;
        v0 += 1;
        v2 = k;
        v1 = v14;
    } while (k >= 9);
    if (v17 != 0x1000000)
        return 0;
    v31 = 0;
    if (!idx->field_84)
        return 1;
    do
    {
        if (*((char *)(v31 + a2)))
        {
            *((unsigned int *)(idx->field_88 + (&v8)[*((char *)(v31 + a2))] * 4)) = v31;
            idx1 = *((char *)(v31 + a2));
            (&v8)[idx1] = (&v8)[idx1] + 1;
        }
    } while ((v31 += 1, v31 < idx->field_84));
    return 1;
}
