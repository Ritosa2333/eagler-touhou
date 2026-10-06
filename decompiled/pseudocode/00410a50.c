// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410a50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410a50_3 {
    struct st_410a50_1 *field_0;
} st_410a50_3;

typedef struct st_410a50_0 {
    char padding_0[64];
    unsigned int field_40;
} st_410a50_0;

typedef struct st_410a50_4 {
    struct st_410a50_2 *field_0;
    struct st_410a50_4 *field_4;
} st_410a50_4;

typedef struct st_410a50_1 {
    char padding_0[1148];
    unsigned int field_47c;
} st_410a50_1;

typedef struct st_410a50_2 {
    char padding_0[20];
    struct st_410a50_3 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_410a50_2;

extern char g_4a3738;
extern unsigned int g_4ce8cc;

int sub_410a50(void)
{
    st_410a50_0 *v5;  // eax
    st_410a50_0 *iter;  // edx
    st_410a50_1 **v15;  // eax
    unsigned int v7;  // ebp
    unsigned int v8;  // ebp
    unsigned int v9;  // ecx
    st_410a50_4 *iter1;  // eax
    st_410a50_2 *v11;  // esi
    st_410a50_4 *node;  // eax
    st_410a50_2 *idx;  // eax
    st_410a50_1 **v14;  // eax
    int v0;  // [bp-0x14]
    int v1;  // [bp-0x10]
    int v2;  // [bp-0xc]
    int v3;  // [bp-0x8]

    sub_464c40(v0, v1, v2, v3, &v5[3].padding_0[4]);
    iter = &v5->field_40;
    v7 = 5;
    do
    {
        v8 = v7;
        v9 = *((int *)&iter->padding_0[0]);
        if (v9)
        {
            iter1 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    v11 = iter1->field_0;
                    if (*((int *)&v11->padding_0[0]) == v9)
                        goto LABEL_410ab6;
                } while ((iter1 = (st_410a50_4 *)iter1->field_4, iter1));
            }
            node = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                while (1)
                {
                    v11 = node->field_0;
                    if (*((int *)&v11->padding_0[0]) == v9)
                        break;
                    node = node->field_4;
                }
LABEL_410ab6:
                idx = v11;
                if (idx)
                {
                    idx->field_47c = idx->field_47c | 0x10000000;
                    if (!idx->field_18)
                    {
                        v14 = idx->field_14;
                        if (idx->field_14)
                        {
                            do
                            {
                                v15 = v14;
                                *(v15)->field_47c = *(v15)->field_47c | 0x10000000;
                                v14 = v15[1];
                            } while (v15[1]);
                        }
                    }
                }
            }
        }
    } while ((*((unsigned int *)&iter->padding_0[0]) = 0, iter += 4, v7 = v8 - 1, v8 != 1));
    *((char **)&v5[3].padding_0[4]) = &g_4a3738;
    sub_464c40();
    return;
}
