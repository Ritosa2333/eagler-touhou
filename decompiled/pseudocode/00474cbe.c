// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474cbe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474cbe_0 {
    char padding_0[72];
    int field_48;
} st_474cbe_0;

extern void g_49d43c;
extern void g_49d46c;

int sub_474cbe(void)
{
    int v7;  // ebx
    unsigned int v8;  // edi
    int v17;  // edi
    char *v18;  // edi
    st_474cbe_0 *v19;  // edi
    st_474cbe_0 *v9;  // edx
    unsigned int v10;  // ecx
    char *v11;  // ecx
    char *v12;  // edi
    char *v13;  // ebx
    char *i;  // eax
    void* v15;  // esi
    char *v16;  // ebx
    unsigned int v0;  // [bp-0xa8]
    st_474cbe_0 *v1;  // [bp-0x9c]
    int v2;  // [bp-0x98]
    char *v3;  // [bp-0x94], Other Possible Types: unsigned int
    unsigned int iter;  // [bp-0x90]
    char v5;  // [bp-0x8c]
    int v6;  // [bp+0x4]

    v7 = 0;
    v0 = v8;
    v1 = v9;
    if (!v6)
    {
        v3 = 1;
        iter = 0;
        if (v11)
        {
            if (*(v11) == 76 && v11[1] == 67 && v11[2] == 95)
            {
                v12 = v11;
                do
                {
                    v13 = sub_488df0(v12, "=;");
                    if (!v13 || !(i = (char *)(v13 - v12), v3 = (char *)(v13 - v12), v13 != v12 && *(v13) != 59))
                    {
LABEL_474e50:
                        return;
                    }
                    v2 = 1;
                    for (v15 = &g_49d43c; (sub_46dc03(*((int *)v15), v12, i) || v3 != sub_475860(*((int *)v15))) && (v2 = (int)(v2 + 1), v15 += 12, v15 + 12 <= &g_49d46c); i = v3);
                    v16 = v13 + 1;
                    v17 = sub_4859c0(v16, ";");
                    if (!v17 && *(v16) != 59)
                        goto LABEL_474e50;
                    if (v2 <= 5)
                    {
                        if (sub_4830fd(&v5, 131, v16, v17))
                            sub_470dc6(0, 0, 0, 0, 0);
                        (&v5)[v17] = 0;
                        if (sub_4749bc(v2))
                            iter += 1;
                    }
                    v18 = &v16[v17];
                } while (*(v18) && (v12 = v18 + 1, *(v12)));
                if (!iter)
                    return;
            }
            else
            {
                if (sub_47478b(v11, &v5, 131, 0, 0, 0))
                {
                    v19 = &v9->field_48;
                    do
                    {
                        if (v7)
                        {
                            if (sub_472ac0(&v5, *((int *)&v19->padding_0[0])))
                            {
                                if (!sub_4749bc(v7))
                                    v3 = 0;
                                else
                                    iter += 1;
                            }
                            else
                            {
                                iter += 1;
                            }
                        }
                    } while ((v7 = (int)(v7 + 1), v19 += 16, v7 <= 5));
                    if (!v3 && !iter)
                        return;
                }
                else
                {
                    return;
                }
            }
        }
        sub_47460e();
        return;
    }
    else if (v10)
    {
        sub_4749bc(v6);
        return;
    }
    else
    {
        return;
    }
}
