// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485797; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_485797_1 {
    char padding_0[212];
    int field_d4;
} st_485797_1;

typedef struct st_485797_0 {
    char padding_0[112];
    unsigned int field_70;
} st_485797_0;

unsigned int sub_485797(char *iter, unsigned int a1, unsigned int a2, unsigned int a3, int a4, int a5)
{
    int v6;  // ebx
    int v7;  // edi
    unsigned int v8;  // eax
    unsigned int v9;  // esi
    unsigned int v10;  // eax
    unsigned int v11;  // esi
    int v12;  // eax
    int v13;  // esi
    st_485797_1 *v0;  // [bp-0x24]
    st_485797_0 *idx;  // [bp-0x1c]
    char v2;  // [bp-0x18]
    int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int node;  // [bp-0x8]

    v4 = 0;
    sub_46d3b4(a5, v6);
    if (!iter)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (!v2)
            return 0;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0;
    }
    else
    {
        if (!a1)
        {
            *((unsigned int *)sub_46e96f(v7)) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            if (v2)
                idx->field_70 = idx->field_70 & 0xfffffffd;
            v8 = 0;
        }
        else
        {
            v9 = a2;
            *(iter) = 0;
            if (!v9)
            {
LABEL_485910:
                *((unsigned int *)sub_46e96f(v13)) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                goto LABEL_485928;
            }
            if (!a4)
                a4 = v0->field_d4;
            v3 = a4;
            node = a1;
            if (a1 > NULL)
            {
                while (1)
                {
                    if (!*((char *)v9))
                    {
LABEL_4858c6:
                        if (node <= 0)
                            goto LABEL_4858e6;
                        *(iter) = 0;
                        v8 = a1 - node;
                        if (v2)
                        {
                            idx->field_70 = idx->field_70 & 0xfffffffd;
                            goto LABEL_485936;
                        }
                    }
                    if (*((char *)v9) != 37)
                    {
                        if (sub_47c5a3(*((char *)v9), &v0) && node > 1)
                        {
                            v10 = v9 + 1;
                            if (*((char *)v10))
                            {
                                *(iter) = *((char *)v9);
                                iter += 1;
                                node -= 1;
                                v9 = v10;
                            }
                            else
                            {
                                v4 = 1;
                                goto LABEL_4858e6;
                            }
                        }
                        *(iter) = *((char *)v9);
                        iter += 1;
                        v9 += 1;
                        node -= 1;
                        goto LABEL_4858c1;
                    }
                    if (!a3)
                        goto LABEL_485910;
                    v11 = v9 + 1;
                    v12 = 0;
                    if (*((char *)v11) == 35)
                    {
                        v12 = 1;
                        v11 += 1;
                    }
                    if (!sub_484f4e(&v0, &node, v3, v12))
                        break;
                    v9 = v11 + 1;
LABEL_4858c1:
                    if (node <= 0)
                        goto LABEL_4858c6;
                }
                if (node > NULL)
                    v4 = 1;
            }
LABEL_4858e6:
            *(iter) = 0;
            if (v4 || node > 0)
                goto LABEL_485910;
            *((unsigned int *)sub_46e96f()) = 0x22;
LABEL_485928:
            if (v2)
                idx->field_70 = idx->field_70 & 0xfffffffd;
            v8 = 0;
LABEL_485936:
        }
        return v8;
    }
}
