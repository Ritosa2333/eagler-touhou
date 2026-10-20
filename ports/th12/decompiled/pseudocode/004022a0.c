// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4022a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4022a0_0 {
    char padding_0[2428];
    unsigned int field_97c;
    char padding_980[296];
    unsigned int field_aa8;
    char padding_aac[99536];
    int field_18f7c;
} st_4022a0_0;

int sub_4022a0(unsigned int a0, unsigned int a1, st_4022a0_0 *a2)
{
    int iter;  // esi
    int i;  // eax
    unsigned int v13;  // ecx
    st_4022a0_0 *j;  // esi
    st_4022a0_0 *v5;  // ecx
    unsigned int v6;  // ebx
    unsigned int v7;  // edi
    st_4022a0_0 *node;  // edi
    st_4022a0_0 *iter1;  // edx
    st_4022a0_0 *v10;  // ebx
    unsigned int v11;  // 4098
    unsigned int v12;  // 4105
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    st_4022a0_0 *v2;  // [bp-0x4], Other Possible Types: unsigned int

    v2 = a0;
    iter = 0;
    i = 0;
    a2 = 0;
    v5 = &a2->field_97c;
    if (a2->field_18f7c > 0)
    {
        v1 = v6;
        v0 = v7;
        node = v5;
        iter1 = &v5->padding_0[300];
        v2 = node;
        v10 = v5;
        do
        {
            v11 = *((int *)&iter1->padding_0[0]);
            *((unsigned int *)&iter1->padding_0[0]) = *((int *)&iter1->padding_0[0]) - 1;
            v12 = _ccall(8, 3, v11, 0xffffffff, 0);
            if (!(v12 & 1))
            {
                if (iter != i)
                {
                    v13 = 78;
                    for (j = v10; v13; j = &j->padding_0[4])
                    {
                        v13 -= 1;
                        *((int *)&node->padding_0[0]) = *((int *)&j->padding_0[0]);
                        node = &node->padding_0[4];
                    }
                    iter = a2;
                }
                iter += 1;
                node = &v2->padding_0[312];
                a2 = iter;
                v2 = node;
            }
            i += 1;
            v10 = &v10->padding_0[312];
            iter1 = &iter1->padding_0[312];
        } while (i < a2->field_18f7c);
    }
    a2->field_18f7c = iter;
    return i;
}
