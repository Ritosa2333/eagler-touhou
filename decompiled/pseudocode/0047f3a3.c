// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f3a3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

extern unsigned int g_4b4318;

void* sub_47f3a3(void* a0, unsigned int *a1)
{
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // eax
    unsigned int v9;  // edx
    unsigned int v11;  // eax
    unsigned int v12;  // edx
    void* v14;  // eax
    unsigned int v15;  // eax
    char *v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3[2];  // [bp-0x1c]
    st_47e7bf_0 v4;  // [bp-0x14]
    unsigned int v5[2];  // [bp-0xc]

    v2 = v6;
    v1 = v7;
    *((unsigned int *)a0) = *(a1);
    v8 = a1[1];
    *((unsigned int *)&a0[4]) = v8;
    if ((char)v8 > 1)
    {
        return a0;
    }
    else if (*((char *)g_4b4318))
    {
        sub_47e0c2(v5);
        v11 = sub_47e9ae(sub_47ec6e(v5, v9, v3, 32), v9, &v4, a0);
        sub_47dde0(v11);
        if ((char)a0[4] > 1)
        {
            return a0;
        }
        else if (*((char *)g_4b4318) != 64)
        {
            v0 = "{for ";
            while (1)
            {
                sub_47ea48(a0, v12, v0);
                do
                {
                    if ((char)a0[4] <= 1 && *((char *)g_4b4318) && *((char *)g_4b4318) != 64)
                    {
                        v14 = sub_47ec6e(sub_47ec02(v5, 96, sub_4814b0(&v4, v3, 39)), v9, v3, 39);
                        sub_47e7bf(a0, v9, v14);
                        v15 = g_4b4318;
                        if (*((char *)v15) == 64)
                        {
                            v15 += 1;
                            g_4b4318 = v15;
                        }
                        if ((char)a0[4] > 1)
                            goto LABEL_47f4a8;
                    }
                    else
                    {
                        if ((char)a0[4] <= 1)
                        {
                            if (!*((char *)g_4b4318))
                                sub_47e4af(1);
                            sub_47e9f6(125);
                        }
LABEL_47f4a8:
                        if (*((char *)g_4b4318) != 64)
                            return a0;
                        g_4b4318 = g_4b4318 + 1;
                        return a0;
                    }
                } while (*((char *)v15) == 64);
                v0 = "s ";
            }
        }
        else
        {
            g_4b4318 = g_4b4318 + 1;
            return a0;
        }
    }
    else
    {
        if ((char)v8 > 1)
            return a0;
        sub_47dde0(sub_47ec26(v3, 1, a0));
        return a0;
    }
}
