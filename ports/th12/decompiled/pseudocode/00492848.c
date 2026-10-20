// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492848; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492848_2 {
    char field_0;
    char padding_1[7];
    char field_8;
    char padding_9[11];
    unsigned int field_14;
    int field_18;
} st_492848_2;

typedef struct st_492848_0 {
    char padding_0[8];
    char field_8;
} st_492848_0;

typedef struct st_492848_1 {
    int field_0;
    struct st_492848_0 *field_4;
    unsigned int field_8;
} st_492848_1;

extern int g_4ab518;

int sub_492848(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    void* idx;  // ebp
    st_492848_1 *v2;  // eax
    int v3;  // eax
    void* v4;  // esi
    unsigned int v5;  // eax
    st_492848_2 *v6;  // edi
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4ab518, 12);
    *((unsigned int *)((char *)idx - 28)) = 0;
    v2 = (int)idx[16];
    if (v2->field_4 && v2->field_4->field_8 && (v2->field_8 || v2->field_0 & 0x80000000))
    {
        v3 = v2->field_0;
        v4 = (int)idx[12];
        if (v3 >= NULL)
            v4 = v2->field_8 + (int)idx[12] + 12;
        *((unsigned int *)((char *)idx - 4)) = 0;
        v0 = 1;
        if (!((char)v3 & 8))
        {
            v6 = (int)idx[20];
            if (v6->field_0 & 1)
            {
                if (sub_49319c(*((int *)((int)idx[8] + 24))) && sub_4931ae(v4, 1))
                {
                    sub_47a6a0(v4, *((int *)((int)idx[8] + 24)), v6->field_14);
                    if (v6->field_14 != 4)
                    {
                        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                        return sub_46fd31();
                    }
                    v5 = *((int *)v4);
                    if (!*((int *)v4))
                    {
                        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                        return sub_46fd31();
                    }
                    v0 = &v6->field_8;
                    v0 = &v6->field_8;
                    *((unsigned int *)v4) = sub_4921d6(v5, v0);
                    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                    return sub_46fd31();
                }
            }
            else
            {
                if (!v6->field_18)
                {
                    if (sub_49319c() && sub_4931ae(v4, 1))
                    {
                        v0 = v6->field_14;
                        sub_47a6a0(v4, sub_4921d6(*((int *)((int)idx[8] + 24)), &v6->field_8));
                        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                        return sub_46fd31();
                    }
                }
                else
                {
                    if (sub_49319c() && sub_4931ae(v4, 1) && sub_4931c0(v6->field_18))
                    {
                        v0 = 0;
                        *((unsigned int *)((char *)idx - 28)) = (unsigned int)((v6->field_0 & 4) + 1);
                        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                        return sub_46fd31();
                    }
                }
            }
        }
        else if (sub_49319c(*((int *)((int)idx[8] + 24))) && sub_4931ae(v4, 1))
        {
            v5 = *((int *)((int)idx[8] + 24));
            *((unsigned int *)v4) = v5;
            v0 = (int)idx[20] + 8;
            *((unsigned int *)v4) = sub_4921d6(v5, v0);
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
            return sub_46fd31();
        }
        sub_472b94(); /* do not return */
    }
    return sub_46fd31();
}
