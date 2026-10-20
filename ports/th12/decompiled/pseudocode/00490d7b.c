// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490d7b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_490d7b_0 {
    char field_0;
    char field_1;
} st_490d7b_0;

typedef struct st_490d7b_2 {
    struct st_490d7b_0 *field_0;
    unsigned int field_4;
} st_490d7b_2;

typedef struct st_490d7b_4 {
    unsigned int field_0;
} st_490d7b_4;

extern st_490d7b_4 *g_4b3d80;
extern char g_4b3d84;
extern st_490d7b_4 *g_4b3d88;

unsigned int sub_490d7b(st_490d7b_0 **a0, unsigned int a1)
{
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    unsigned int v19;  // edi
    unsigned int v20;  // eax
    char *v21;  // eax
    unsigned int v10;  // ecx
    unsigned int *v11;  // eax
    int v12;  // edi
    st_490d7b_0 **i;  // esi
    unsigned int v14;  // eax
    int v15;  // eax
    st_490d7b_2 *v16;  // ecx
    char *v17;  // esi
    char *v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    char *v3;  // [bp-0x18]
    unsigned int *v4;  // [bp-0x14]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    char *v7;  // [bp-0x8]

    v5 = 0;
    if (!a0)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 0xffffffff;
    }
    v2 = v8;
    v1 = v9;
    v7 = &*(a0)->field_0;
    if (v7 && !(v10 = 61, v3 = (char *)sub_49143d(v7, 61), !v3 || v7 == v3))
    {
        v6 = !v3[1];
        v11 = &g_4b3d80->field_0;
        if (v11 == *((int *)&g_4b3d84))
        {
            v11 = sub_490d21();
            g_4b3d80 = v11;
        }
        if (!v11)
        {
            if (a1 && g_4b3d88)
            {
                if (sub_48e343(v10))
                    goto LABEL_490e0b;
            }
            else
            {
                if (!v6)
                {
                    g_4b3d80 = sub_4777ce(4);
                    if (!g_4b3d80)
                        return 0xffffffff;
                    g_4b3d80->field_0 = 0;
                    if (!g_4b3d88)
                    {
                        g_4b3d88 = sub_4777ce(4);
                        if (!g_4b3d88)
                            return 0xffffffff;
                        g_4b3d88->field_0 = 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
        }
        v4 = &g_4b3d80->field_0;
        if (g_4b3d80)
        {
            v12 = sub_490ccf(v7);
            if (v12 >= NULL && g_4b3d80->field_0)
            {
                i = &g_4b3d80[v12];
                sub_46c941(*(i));
                if (!v6)
                {
                    *(i) = v7;
                    *(a0) = NULL;
                    goto LABEL_490f19;
                }
                else
                {
                    for (; *(i); i = &v4[1 + v12])
                    {
                        *(i) = i[1];
                    }
                    if (v12 < 0x3fffffff && !(v14 = sub_4778ad(g_4b3d80, v12, 4), !v14))
                        goto LABEL_490f14;
                    goto LABEL_490f19;
                }
            }
            else
            {
                if (v6)
                {
                    sub_46c941(v7);
                    *(a0) = NULL;
                    return 0;
                }
                if (v12 < NULL)
                    v12 = -(v12);
                v15 = v12 + 2;
                if (v15 >= v12 && v15 < 0x3fffffff && (v14 = sub_4778ad(g_4b3d80, 4, v15), v14))
                {
                    v16 = v14 + v12 * 4;
                    v16->field_0 = v7;
                    v16->field_4 = 0;
                    *(a0) = NULL;
LABEL_490f14:
                    g_4b3d80 = v14;
LABEL_490f19:
                    if (a1)
                    {
                        v17 = v7;
                        v19 = sub_477813(sub_475860(v17) + 2, 1);
                        if (v19)
                        {
                            v0 = v17;
                            v20 = sub_475860(v17);
                            if (sub_47a2b9(v19, v20 + 2))
                                sub_470dc6(0, 0, 0, 0, 0);
                            v21 = v19 - v17 + v3;
                            *(v21) = 0;
                            if (!SetEnvironmentVariableA(v19, ~(-(0 < v6)) & v21 + 1))
                            {
                                v5 = 0xffffffff;
                                *((unsigned int *)sub_46e96f()) = 42;
                            }
                            sub_46c941(v19);
                        }
                    }
                    if (!v6)
                        return v5;
                    sub_46c941(v7);
                    *(a0) = NULL;
                    return v5;
                }
            }
        }
    }
    else
    {
LABEL_490e0b:
        *((unsigned int *)sub_46e96f()) = 22;
    }
    return 0xffffffff;
}
