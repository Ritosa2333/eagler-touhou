// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc9d4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc9d4_0 {
    char padding_0[608];
    void* field_260;
    char field_264;
} st_4dc9d4_0;


unsigned int sub_4dc9d4(st_4dc9d4_0 *idx, unsigned int a1)
{
    unsigned int v12;  // ebx
    unsigned int v13;  // esi
    unsigned int v23;  // eax
    int node;  // esi
    int v25;  // eax
    int v26;  // eax
    int v28;  // eax
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    st_4dc9d4_0 *v14;  // ebp
    unsigned int v32;  // eax
    void* iter;  // edi
    char *l;  // esi
    unsigned int v35;  // ecx
    unsigned int v15;  // edi
    void* iter1;  // edi
    unsigned int v18;  // ecx
    unsigned int j;  // esi
    unsigned int v20;  // edx
    unsigned int v21;  // edx
    unsigned int v0;  // [bp-0x31c]
    unsigned int v1;  // [bp-0x318]
    unsigned int v2;  // [bp-0x310]
    unsigned short v3;  // [bp-0x30c]
    char v4;  // [bp-0x308]
    int <0x4dc9d4[is_5]|Stack bp-0x2fd, 1 B>;  // [bp-0x2fd]
    int v5;  // [bp-0x2fd]
    int v6;  // [bp-0x2fd]
    int v7;  // [bp-0x2fd]
    char v8;  // [bp-0x2f]
    char v9;  // [bp-0x1b]
    char v10;  // [bp-0x17]

    v2 = v12;
    v1 = v13;
    v14 = &idx->padding_0[4];
    v0 = v15;
    if (!sub_4dc615(v14, a1, 1))
    {
        iter1 = idx->field_260;
        for (v18 = 189; v18; iter1 += 4)
        {
            v18 -= 1;
            *((unsigned int *)iter1) = 0;
        }
        *((char *)iter1) = 0;
    }
    j = 0;
    do
    {
        *((char *)&v3 + j) = sub_4dc615(v14, v21, 4);
        j += 1;
    } while (j < 19);
    v23 = sub_4dc6a5(&v3);
    if (!(char)v23)
        return v23;
    node = 0;
    do
    {
        v25 = sub_4dc821(&idx->padding_0[448]);
        if (v25 < 16)
        {
            *((char *)&<0x4dc9d4[is_5]|Stack bp-0x2fd, 1 B> + node + 1) = *((char *)idx->field_260 + node) + (char)v25 & 15;
            node += 1;
            v6 = v5;
        }
        else if (v25 == 16)
        {
            v26 = sub_4dc615(v14, v20, 2) + 3;
            v7 = v5;
            v6 = v5;
            if (v26 > 0)
            {
                do
                {
                    v6 = v7;
                    if (node >= 757)
                        goto LABEL_4dcac3;
                } while ((v26 = (int)(v26 - 1), *((char *)((char *)&v6 + node + 1)) = *((char *)((char *)&v6 + node)), node = (int)(node + 1), v6 = v7, v26 > 0));
            }
        }
        else
        {
            v28 = (v25 == 0x11 ? sub_4dc615(v14, v20, 3) + 3 : sub_4dc615(v14, v20, 7) + 11);
            v6 = v5;
            if (v28 > 0)
            {
                do
                {
                    v6 = v5;
                    if (node >= 757)
                        goto LABEL_4dcac3;
                } while ((*((char *)((char *)&<0x4dc9d4[is_5]|Stack bp-0x2fd, 1 B> + node + 1)) = 0, node = (int)(node + 1), v28 = (int)(v28 - 1), v6 = v5, v28 > 0));
            }
        }
    } while (node < 757);
LABEL_4dcac3:
    v29 = sub_4dc6a5(&v6 + 1);
    if (!(char)v29)
        return v29;
    v30 = sub_4dc6a5(&v8);
    if (!(char)v30)
        return v30;
    v31 = sub_4dc6a5(&v10);
    if (!(char)v31)
        return v31;
    idx->field_264 = 0;
    v32 = 0;
    do
    {
        if ((&v9)[v32] != 3)
        {
            idx->field_264 = 1;
            break;
        }
    } while ((v32 += 1, v32 < 8));
    iter = idx->field_260;
    l = &v4;
    for (v35 = 757; v35; l += 1)
    {
        v35 -= 1;
        *((char *)iter) = *(l);
        iter += 1;
    }
    return _INSERT(v32, 0, 1);
}
