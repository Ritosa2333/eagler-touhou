// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dcb71; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dcb71_0 {
    struct st_4dcb71_1 *field_0;
    struct st_4dcb71_1 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[576];
    unsigned int field_250;
    unsigned int field_254;
    unsigned int field_258;
} st_4dcb71_0;

typedef struct st_4dcb71_1 {
    char field_0;
} st_4dcb71_1;

extern char g_46b49a;
extern char g_46b4b6;

char sub_4dcb71(st_4dcb71_0 *a0, unsigned int a1, unsigned int a2, unsigned int *a3)
{
    unsigned int iter;  // edi
    st_4dcb71_0 *idx;  // esi
    unsigned int v15;  // eax
    unsigned int v16;  // ecx
    unsigned int v17;  // edx
    unsigned int v18;  // ebx
    char v19;  // al
    unsigned int v21;  // edi
    unsigned int v22;  // ebp
    unsigned int v23;  // edx
    unsigned int v24;  // ecx
    int v7;  // eax
    unsigned int j;  // ecx
    unsigned int v26;  // edi
    unsigned int v27;  // ebx
    unsigned int v28;  // edx
    unsigned int v29;  // ecx
    unsigned int k;  // ecx
    unsigned int v31;  // eax
    unsigned int v32;  // ecx
    unsigned int v33;  // edx
    char *l;  // eax
    int v8;  // eax
    unsigned int v35;  // edi
    char *v36;  // edx
    char *v37;  // eax
    unsigned int v9;  // eax
    unsigned int v10;  // ebp
    unsigned int v11;  // eax
    unsigned int v12;  // ebx
    unsigned int v13;  // ecx
    unsigned int i;  // edx
    unsigned int node;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    *(a3) = 0;
    iter = 0;
    idx = a0;
    node = 0;
    if (!a2)
        return 1;
    while (1)
    {
        v7 = sub_4dc821(idx->padding_10);
        if (v7 < 0x100)
        {
            idx->field_0->field_0 = v7;
            iter += 1;
            idx->field_0 = idx->field_0 + 1;
            node = iter;
            goto LABEL_4dcde1;
        }
        if (v7 < 720)
            break;
        if (!(char)sub_4dc9d4(idx, v17))
            return 0;
LABEL_4dcde1:
        if (iter >= a2)
        {
            *(a3) = iter;
            return 1;
        }
    }
    v8 = v7 - 0x100;
    v9 = v8 & 7;
    v10 = (unsigned int)(v8) >> 3;
    v1 = v9 + 2;
    if (v9 == 7)
    {
        sub_4dc821(&idx->padding_10[144]);
        v11 = sub_4dcb63(idx);
        v12 = *((char *)(v11 + (char *)idx + &g_46b4b6));
        if (v13 >= 8)
        {
            do
            {
                *((char *)&v2) = idx->field_4->field_0;
                idx->field_4 = idx->field_4 + 1;
                i = idx->field_8 - 8;
                idx->field_c = idx->field_c * 0x100 | v2 & 0xff;
                idx->field_8 = i;
            } while (i >= 8);
        }
        idx->field_8 = idx->field_8 + v12;
        v15 = sub_4dcb63(idx);
        v16 = _INSERT(0, 0, *((char *)(v15 + (char *)idx + &g_46b49a)));
        v1 += v16 + v17;
    }
    v18 = (&idx[1].field_c)[v10];
    v19 = sub_4dcb63(idx);
    v21 = _INSERT(0, 0, *((char *)(v10 + (char *)idx + L"I\x86\x00蘌耀")));
    if (v19 && v21 >= 3)
    {
        v22 = v21 - 3;
        if (idx->field_8 >= 8)
        {
            do
            {
                v23 = idx->field_c;
                *((char *)&v3) = idx->field_4->field_0;
                v24 = idx->field_8;
                idx->field_4 = idx->field_4 + 1;
                j = v24 - 8;
                idx->field_c = v23 * 0x100 | v3 & 0xff;
                idx->field_8 = j;
            } while (j >= 8);
        }
        v26 = idx->field_c >> ((char)(8 - idx->field_8) & 31);
        idx->field_8 = idx->field_8 + v22;
        v27 = sub_4dc821(&idx->padding_10[288]) + v18 + ((v26 & 0xffffff) >> ((char)(24 - v22) & 31)) * 8;
    }
    else
    {
        if (idx->field_8 >= 8)
        {
            do
            {
                v28 = idx->field_c;
                *((char *)&v4) = idx->field_4->field_0;
                v29 = idx->field_8;
                idx->field_4 = idx->field_4 + 1;
                k = v29 - 8;
                idx->field_c = v28 * 0x100 | v4 & 0xff;
                idx->field_8 = k;
            } while (k >= 8);
        }
        v31 = idx->field_c >> ((char)(8 - idx->field_8) & 31);
        idx->field_8 = idx->field_8 + v21;
        v27 = v18 + ((v31 & 0xffffff) >> ((char)(24 - v21) & 31));
    }
    if (v27 < 3)
    {
        v32 = (&idx->field_250)[v27];
        if (!v27)
            goto LABEL_4dcda8;
        (&idx->field_250)[v27] = idx->field_250;
    }
    else
    {
        v33 = idx->field_250;
        v32 = v27 - 3;
        idx->field_258 = idx->field_254;
        idx->field_254 = v33;
    }
    idx->field_250 = v32;
LABEL_4dcda8:
    l = &idx->field_0->field_0;
    v35 = v1;
    v36 = &l[v35];
    idx->field_0 = v36;
    if (l < v36)
    {
        do
        {
            v37 = l + 1;
            *(v37 - 1) = *(&l[-1 * v32] - 1);
            l = v37;
        } while (l < idx->field_0);
    }
    node += v35;
    iter = node;
    goto LABEL_4dcde1;
}
