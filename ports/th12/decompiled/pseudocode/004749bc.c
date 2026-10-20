// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4749bc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4749bc_0 {
    char padding_0[12];
    unsigned int field_c;
} st_4749bc_0;

extern unsigned int g_49d438[4];
extern int g_49d478;
extern int g_4ad9d0;

int sub_4749bc(void)
{
    unsigned int v15;  // edi
    unsigned int *idx;  // edi
    unsigned int v25;  // edx
    unsigned int *idx1;  // eax
    unsigned int idx2;  // eax
    void* v28;  // edi
    int v17;  // ecx
    void* index;  // esi
    void* v19;  // ebx
    int v21;  // ecx
    unsigned int v22;  // ecx
    unsigned int *iter;  // eax
    unsigned int v24;  // edx
    int v0;  // [bp-0x1cc]
    char v1;  // [bp-0x1c8]
    unsigned int v2;  // [bp-0x1c0]
    unsigned int v3;  // [bp-0x1b8]
    char v4;  // [bp-0x1b4], Other Possible Types: unsigned short
    unsigned int v5;  // [bp-0x1ac]
    unsigned int v6;  // [bp-0x1a8]
    void* v7;  // [bp-0x1a0], Other Possible Types: unsigned int
    char v8;  // [bp-0x19c], Other Possible Types: unsigned int
    st_4749bc_0 *v9;  // [bp-0x198]
    unsigned int v10;  // [bp-0x194]
    int i;  // [bp-0x190]
    char v12;  // [bp-0x18c]
    char v13;  // [bp-0x8c]
    int v14;  // [bp+0x4]

    idx = sub_475467(v15, v0) + 464;
    if (!sub_47478b(v17, &v13, 131, &v4, &v8, v14))
        return;
    v19 = v14 * 16 + index;
    if (!sub_472ac0(&v13, (int)v19[72]))
        return;
    i = sub_475860(&v13) + 5;
    v10 = sub_4777ce(i);
    if (v10)
    {
        v21 = v14;
        v6 = (int)v19[72];
        v9 = index + v21 * 4 + 12;
        v5 = *((int *)&v9->padding_0[0]);
        v7 = (v21 + 6) * 6 + index;
        sub_47a330(&v1, v7, 6);
        v3 = (int)index[4];
        if (sub_47a2b9(v10 + 4, i - 4, &v13))
            sub_470dc6(0, 0, 0, 0, 0);
        *((unsigned int *)&v19[72]) = v10 + 4;
        *((unsigned int *)&v9->padding_0[0]) = v4;
        sub_47a330(v7, &v4, 6);
        if (v14 == 2)
        {
            i = 0;
            *((unsigned int *)&index[4]) = v8;
            v22 = idx[8];
            v7 = idx[9];
            iter = idx;
            do
            {
                if ((int)index[4] == *(iter) && i)
                {
                    idx1 = &idx[2 * i];
                    *(idx) = *(idx1);
                    idx[1] = idx1[1];
                    *(idx1) = v22;
                    idx1[1] = v7;
                    break;
                }
                v24 = *(iter);
                i += 1;
                *(iter) = v22;
                v2 = v24;
                v25 = iter[1];
                iter[1] = v7;
                v22 = v2;
                iter += 2;
                v7 = v25;
            } while (i < 5);
            if (i == 5)
            {
                if (sub_48384c(0, 1, &g_49d478, 127, &v12, (int)index[4], (int)index[20], 1))
                {
                    idx2 = 0;
                    do
                    {
                        *((unsigned short *)&(&v12)[2 * idx2]) = *((short *)&(&v12)[2 * idx2]) & 0x1ff;
                        idx2 += 1;
                    } while (idx2 < 127);
                    idx[1] = -(0 < sub_487768(&v12, g_4ad9d0, 254)) + 1;
                }
                else
                {
                    idx[1] = 0;
                }
                *(idx) = (int)index[4];
            }
            *((unsigned int *)&index[168]) = idx[1];
        }
        if (v14 == 1)
            *((unsigned int *)&index[8]) = v8;
        if (g_49d438[3 * v14](index))
        {
            *((unsigned int *)&v19[72]) = v6;
            sub_46c941(v10, v15);
            *((unsigned int *)&v9->padding_0[0]) = v5;
            *((unsigned int *)&index[4]) = v3;
        }
        else
        {
            if (v6 != "C")
            {
                v28 = (v14 + 5) * 16 + index;
                if (!InterlockedDecrement(*((int *)v28)))
                {
                    sub_46c941(*((int *)v28), v15);
                    sub_46c941((int)v19[84], *((int *)v28));
                    *((unsigned int *)&v19[76]) = 0;
                }
            }
            *((unsigned int *)v10) = 1;
            *((unsigned int *)(16 * v14 + (char *)index + 80)) = v10;
            return;
        }
    }
    return;
}
