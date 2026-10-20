// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ac02; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ac02_0 {
    char field_0;
} st_47ac02_0;

extern st_47ac02_0 *g_4b38d0;
extern int g_4b3d80;
extern unsigned int g_4d6428;
extern unsigned int g_4d6434;

unsigned int sub_47ac02(void)
{
    unsigned int v4;  // esi
    char *i;  // esi
    unsigned int v6;  // edi
    int v7;  // edi
    unsigned int iter;  // edi
    char *v9;  // esi
    unsigned int v10;  // ebx
    int v11;  // ebx
    int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (!g_4d6434)
        sub_473e82();
    v2 = v4;
    i = &g_4b38d0->field_0;
    v1 = v6;
    if (!g_4b38d0)
        return 0xffffffff;
    for (v7 = 0; *(i); i = &i[sub_475860(i) + 1])
    {
        if (*(i) != 61)
            v7 += 1;
    }
    iter = sub_477813(v7 + 1, 4);
    g_4b3d80 = iter;
    if (iter)
    {
        v9 = &g_4b38d0->field_0;
        v0 = v10;
        while (1)
        {
            if (*(v9))
            {
                v11 = sub_475860(v9) + 1;
                if (*(v9) != 61)
                {
                    v12 = sub_477813(v11, 1);
                    *((int *)iter) = v12;
                    if (v12)
                    {
                        if (sub_47a2b9(v12, v11, v9))
                            sub_470dc6(0, 0, 0, 0, 0);
                        iter += 4;
                    }
                    else
                    {
                        sub_46c941(g_4b3d80);
                        g_4b3d80 = 0;
                        v13 = 0xffffffff;
                        break;
                    }
                }
                v9 = &v9[v11];
            }
            else
            {
                sub_46c941(g_4b38d0);
                g_4b38d0 = 0;
                *((int *)iter) = 0;
                g_4d6428 = 1;
                v13 = 0;
                break;
            }
        }
        return v13;
    }
    return 0xffffffff;
}
