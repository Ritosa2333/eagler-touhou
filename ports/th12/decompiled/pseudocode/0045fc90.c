// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fc90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fc90_0 {
    int field_0;
    char padding_4[260];
    struct st_45fc90_1 *field_108;
    unsigned int field_10c;
    unsigned int field_110;
    unsigned int field_114;
    unsigned int field_118;
    unsigned int field_11c;
    int field_120;
} st_45fc90_0;

typedef struct st_45fc90_1 {
    char padding_0[4];
    unsigned short field_4;
    unsigned short field_6;
} st_45fc90_1;

extern int g_4a32c4;
extern int g_4a33b8;
extern unsigned int g_4ad138;
extern char g_4b50c0;

int sub_45fc90(char *a0, unsigned int a1, int a2, int a3)
{
    char *v6;  // edi
    int v7;  // edi
    unsigned int v16;  // edx
    unsigned int v17;  // edi
    unsigned int v18;  // ecx
    unsigned short *node;  // eax
    unsigned int v20;  // ebp
    int v21;  // eax
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    int v8;  // ebp
    int v9;  // esi
    int v10;  // ebx
    unsigned short *iter;  // ebx
    st_45fc90_0 *idx;  // esi
    char *iter1;  // eax
    char i;  // 4102
    unsigned int v15;  // ebp
    int v0;  // [bp-0x11c]
    unsigned int v1;  // [bp-0x118]
    unsigned int v2;  // [bp-0x114]
    char v3;  // [bp-0x110]
    char v4;  // [bp-0x108]
    unsigned int v5;  // [bp-0x4]

    v5 = g_4ad138 ^ &v3;
    v6 = a0;
    sub_4622e0("::preloadAnim : %s\n", v6, v7, v8, a2);
    if (a3 >= 32)
    {
        sub_464300(&g_4a32c4);
    }
    else
    {
        sub_46cbbb(&v4, "%s", v6, v9, v10);
        iter = sub_463c10(0, 0);
        v2 = 0;
        idx = sub_46c9ea(312);
        if (idx)
            sub_477420(idx, 0, 312);
        else
            idx = NULL;
        *((st_45fc90_0 **)&(&g_4b50c0)[4 * a3 + v7]) = idx;
        if (iter)
        {
            idx->field_0 = a3;
            idx->field_108 = iter;
            iter1 = v6;
            do
            {
                i = *(iter1);
                *(idx - v6 + iter1 + 4) = *(iter1);
                iter1 += 1;
            } while (i);
            v15 = iter[3];
            v16 = *((int *)&iter[18]);
            v17 = iter[2];
            v18 = 1;
            v1 = v15;
            node = iter;
            if (*((int *)&node[18]))
            {
                do
                {
                    node = (char *)node + v16;
                    v15 += node[3];
                    v17 += node[2];
                    v16 = *((int *)&node[18]);
                    v18 += 1;
                } while (*((int *)&node[18]));
                v1 = v15;
            }
            v20 = v18 * 20;
            idx->field_10c = v18;
            v21 = sub_46d04a(v20);
            idx->field_120 = v21;
            sub_477420(v21, 0, v20);
            idx->field_118 = sub_46d04a(v17 * 72);
            v22 = sub_46d04a(v15 * 4);
            idx->field_114 = v17;
            idx->field_11c = v22;
            idx->field_110 = v15;
            while (1)
            {
                if (iter)
                {
                    if (sub_45fee0(iter) < NULL)
                        goto LABEL_45fe35;
                    v0 += 1;
                    if (*((int *)&iter[18]))
                        iter = (char *)iter + *((int *)&iter[18]);
                    else
                        goto LABEL_45fe35;
                }
                else
                {
                    sub_464300(&g_4a33b8);
                    goto LABEL_45fe35;
                }
            }
        }
LABEL_45fe35:
    }
    _security_check_cookie();
    return v23;
}
