// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4727c2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4727c2_0 {
    char padding_0[4];
    int field_4;
} st_4727c2_0;

typedef struct st_4727c2_2 {
    char padding_0[4];
    struct st_4727c2_1 *field_4;
} st_4727c2_2;

typedef struct st_4727c2_1 {
    unsigned int field_0;
    struct st_4727c2_1 *field_4;
} st_4727c2_1;

extern char g_4aab60;

void sub_4727c2(unsigned int a0)
{
    void* index;  // ebp
    st_4727c2_0 *idx;  // edi
    int v6;  // eax
    int iter;  // esi
    char *v8;  // eax
    st_4727c2_1 *v9;  // ebx
    int v10;  // esi
    int v11;  // eax
    st_4727c2_2 *idx1;  // eax
    unsigned int v0;  // [bp+0xc]
    unsigned int v1;  // [bp+0x10]
    unsigned int v2;  // [bp+0x14]
    unsigned int v3;  // [bp+0x18]

    sub_46fcec(&g_4aab60, 12);
    idx = (int)index[8];
    if (!idx->field_4)
    {
        v6 = sub_4821bc(0, &idx[1].padding_0[1], 0, sub_46d04a, sub_46c941, 0x2800);
        *((int *)((char *)index - 28)) = v6;
        if (v6)
        {
            iter = sub_475860(v6);
            while (1)
            {
                if (iter > 0)
                {
                    iter -= 1;
                    v8 = *((int *)((char *)index - 28)) + iter;
                    if (*(v8) != 32)
                        break;
                    *(v8) = 0;
                }
                else
                {
                    iter -= 1;
                    break;
                }
            }
            sub_46ec9a(14);
            *((unsigned int *)((char *)index - 4)) = 0;
            if (!idx->field_4)
            {
                v9 = sub_46d04a(8);
                if (v9)
                {
                    v10 = iter + 2;
                    v11 = sub_46d04a(v10);
                    idx->field_4 = v11;
                    if (v11)
                    {
                        if (sub_47a2b9(v11, v10, *((int *)((char *)index - 28))))
                            sub_470dc6(0, 0, 0, 0, 0);
                        v9->field_0 = idx->field_4;
                        idx1 = (int)index[12];
                        v9->field_4 = idx1->field_4;
                        idx1->field_4 = v9;
                    }
                    else
                    {
                        sub_46c941(v9, &g_4aab60);
                    }
                }
            }
            sub_46c941(*((int *)((char *)index - 28)), v0);
            *((unsigned int *)((char *)index - 4)) = 0xfffffffe;
            sub_4728ae();
        }
    }
    sub_46fd31(v0, v1, v2, v3);
    return;
}
