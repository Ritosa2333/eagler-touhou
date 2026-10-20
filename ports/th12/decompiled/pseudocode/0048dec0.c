// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48dec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48dec0_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    struct st_48dec0_1 *field_c;
} st_48dec0_0;

typedef struct st_48dec0_2 {
    char field_0;
    char field_1;
} st_48dec0_2;

typedef struct st_48dec0_1 {
    char field_0;
} st_48dec0_1;

unsigned int sub_48dec0(st_48dec0_2 *a0, unsigned int a1, int a2, st_48dec0_0 *a3)
{
    st_48dec0_0 *idx;  // ecx
    char *node;  // edi
    unsigned int v6;  // esi
    int v7;  // edi
    int v8;  // edx
    st_48dec0_2 *iter;  // eax
    char v10;  // cl
    st_48dec0_2 *v11;  // edi
    unsigned int v0;  // [bp-0x14]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    char v3;  // [bp+0x0]

    idx = a3;
    node = &idx->field_c->field_0;
    if (!a0 || a1 <= 0)
    {
        v0 = 22;
        v6 = 22;
        *((unsigned int *)sub_46e96f(v7, v1, v2, &v3)) = 22;
    }
    else
    {
        v8 = a2;
        a0->field_0 = 0;
        if (a1 <= (v8 <= 0 ? 0 : v8) + 1)
        {
            v0 = 0x22;
            *((unsigned int *)sub_46e96f()) = 0x22;
            v6 = 0x22;
        }
        else
        {
            a0->field_0 = 48;
            iter = &a0->field_1;
            if (v8 > 0)
            {
                do
                {
                    if (*(node))
                    {
                        v10 = *(node);
                        node += 1;
                    }
                    else
                    {
                        v0 = 48;
                        v10 = 48;
                    }
                } while ((iter->field_0 = v10, iter += 1, v8 = (int)(v8 - 1), v8 > 0));
                idx = a3;
            }
            iter->field_0 = 0;
            if (v8 >= 0 && *(node) >= 53)
            {
                while (1)
                {
                    iter = (char *)iter - 1;
                    if (iter->field_0 != 57)
                        break;
                    iter->field_0 = 48;
                }
                iter->field_0 = iter->field_0 + 1;
            }
            if (a0->field_0 == 49)
            {
                idx->field_4 = idx->field_4 + 1;
            }
            else
            {
                v11 = &a0->field_1;
                sub_47a6a0(a0, v11, sub_475860(v11) + 1);
            }
            return 0;
        }
    }
    sub_470f2d(0, 0, 0, 0, 0);
    return v6;
}
