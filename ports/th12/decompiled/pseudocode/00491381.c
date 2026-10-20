// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491381; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491381_0 {
    char padding_0[112];
    unsigned int field_70;
} st_491381_0;

typedef struct st_491381_1 {
    char padding_0[8];
    unsigned int field_8;
} st_491381_1;


char * sub_491381(char *a0, unsigned int a1, int a2)
{
    int v4;  // ebx
    char *v5;  // eax
    unsigned int v6;  // esi
    unsigned short v7;  // cx
    char *v8;  // eax
    unsigned int v0;  // [bp-0x20]
    st_491381_1 *v1;  // [bp-0x10]
    st_491381_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(a2, v4);
    v5 = a0;
    if (!v5)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (!v3)
            return NULL;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return NULL;
    }
    else
    {
        v0 = v6;
        if (!v1->field_8)
        {
            v5 = sub_46d1a0(v5, a1);
        }
        else
        {
            while (1)
            {
                v7 = *(v5);
                if (v7)
                {
                    if (v1[2].padding_0[5 + (char)v7] & 4)
                    {
                        v8 = v5 + 1;
                        if (!*(v8))
                        {
LABEL_49142b:
                            if (!v3)
                                return NULL;
                            idx->field_70 = idx->field_70 & 0xfffffffd;
                            return NULL;
                        }
                        if (a1 == (v7 * 0x100 | *(v8)))
                        {
                            v5 = v8 - 1;
                            break;
                        }
                        else
                        {
                            v5 = v8 + 1;
                            continue;
                        }
                    }
                    if (a1 == v7)
                        goto LABEL_491415;
                    v5 += 1;
                }
                else
                {
LABEL_491415:
                    if (a1 != v7)
                        goto LABEL_49142b;
                    break;
                }
            }
        }
        if (!v3)
            return v5;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return v5;
    }
}
