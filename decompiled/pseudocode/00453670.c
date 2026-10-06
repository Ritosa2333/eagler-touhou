// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453670; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453670_0 {
    char padding_0[1];
    char field_1;
} st_453670_0;

typedef struct st_453670_1 {
    char padding_0[6532];
    struct st_453670_2 *field_1984;
} st_453670_1;

typedef struct st_453670_2 {
    char field_0;
} st_453670_2;

int sub_453670(char *a0, unsigned int a1, st_453670_1 *a2)
{
    char *v3;  // esi
    unsigned int v4;  // edi
    char *v13;  // ecx
    char v14;  // 4101
    unsigned int v15;  // cc_dep1
    unsigned int v16;  // cc_dep2
    char v17;  // 4103
    unsigned int v18;  // eax
    unsigned int v19;  // 4111
    unsigned int v20;  // 4120
    unsigned int v21;  // ecx
    unsigned int v22;  // eax
    st_453670_0 *v5;  // eax
    unsigned int v23;  // eax
    char *node;  // eax
    char i;  // 4102
    st_453670_0 *v8;  // eax
    st_453670_0 *iter;  // eax
    char j;  // 4102
    char *v11;  // esi
    char *iter1;  // eax
    int v0;  // [bp-0x8c]
    int v1;  // [bp-0x88]
    char v2;  // [bp-0x84]

    v3 = a0;
    v4 = 0;
    v5 = sub_46d260(v3, 47, v0, v1);
    if (!v5 && !(v5 = (st_453670_0 *)sub_46d260(v3, 92), v5))
    {
        node = v3;
        do
        {
            i = *(node);
            *(&v2 - v3 + node) = *(node);
            node += 1;
        } while (i);
    }
    else
    {
        v8 = &v5->field_1;
        iter = v8;
        do
        {
            j = (char)iter->padding_0;
            *((char [1])(&v2 - &v8->padding_0[0] + &iter->padding_0[0])) = iter->padding_0;
            iter = &iter->field_1;
        } while (j);
    }
    v11 = &a2->field_1984->field_0;
    if (*(v11))
    {
        iter1 = v11;
        do
        {
            v13 = &v2;
            while (1)
            {
                v14 = *(iter1);
                v15 = v14;
                v16 = *(v13);
                if (v14 != *(v13))
                    break;
                if (v14)
                {
                    v17 = iter1[1];
                    v15 = v17;
                    v16 = v13[1];
                    if (v17 != v13[1])
                        break;
                    iter1 += 2;
                    v13 += 2;
                    if (!v17)
                        goto LABEL_453701;
                }
                else
                {
LABEL_453701:
                    v18 = 0;
                    goto LABEL_45370a;
                }
            }
            v19 = _ccall(4, v15, v16, 0);
            v20 = _ccall(12, 0, v19 & 1, v19 & 1);
            v18 = -(v19 & 1) + 1 - (v20 & 1);
LABEL_45370a:
        } while (v18 && (v4 += 1, v21 = v4 * 52, iter1 = v21 + v11, v11[v21]));
    }
    if (v11[52 * v4])
        return v23;
    return v22;
}
