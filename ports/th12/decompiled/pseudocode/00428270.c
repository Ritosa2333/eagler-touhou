// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428270; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428270_5 {
    struct st_428270_0 *field_0;
    struct st_428270_4 *field_4;
    struct st_428270_3 *field_8;
    unsigned int field_c;
    char padding_10[4];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_428270_1 *field_20;
    char padding_24[88];
    char field_7c;
    char field_7d;
} st_428270_5;

typedef struct st_428270_0 {
    char padding_0[8];
    struct st_428270_1 *field_8;
    char padding_c[4];
    struct st_428270_1 *field_10;
} st_428270_0;

typedef struct st_428270_1 {
    unsigned int field_0;
} st_428270_1;

typedef struct st_428270_3 {
    char padding_0[4];
    struct st_428270_4 *field_4;
} st_428270_3;

typedef struct st_428270_7 {
    char padding_0[24];
    struct st_428270_5 *field_18;
    char padding_1c[1096];
    struct st_428270_3 *field_464;
    unsigned int field_468;
} st_428270_7;

typedef struct st_428270_4 {
    char padding_0[8];
    struct st_428270_3 *field_8;
} st_428270_4;

unsigned int sub_428270(unsigned int a0)
{
    st_428270_7 *index;  // edi
    st_428270_5 *idx;  // esi
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v5;  // ebx
    unsigned int v6;  // ftop
    char v7;  // al
    st_428270_3 *v8;  // ebx
    unsigned int v9;  // edx
    unsigned int *v10;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = a0;
    idx = index->field_18;
    if (!idx)
        return 1;
    v0 = v5;
    do
    {
        v7 = idx->field_7c;
        v8 = idx->field_8;
        if (idx->field_7c && (idx->field_7c = v7 + 1, v7 + 1 >= 2))
        {
LABEL_428295:
            idx->field_0->field_10();
            index->field_468 = index->field_468 + 0xffffffff;
            idx->field_4->field_8 = idx->field_8;
            if (idx->field_8)
                idx->field_8->field_4 = idx->field_4;
            if (index->field_464 == idx)
            {
                index->field_464 = idx->field_4;
                goto LABEL_4282cb;
            }
        }
        else
        {
            if (idx->field_c == 1)
            {
                idx->field_0->field_10();
                index->field_468 = index->field_468 + 0xffffffff;
                idx->field_4->field_8 = idx->field_8;
                if (idx->field_8)
                    idx->field_8->field_4 = idx->field_4;
                if (index->field_464 == idx)
                {
                    index->field_464 = idx->field_4;
                    sub_46ca4f(idx);
                    continue;
                }
LABEL_4282cb:
                sub_46ca4f(idx);
                continue;
            }
            if (idx->field_0->field_8())
                goto LABEL_428295;
            v9 = idx->field_18;
            v10 = &idx->field_20->field_0;
            idx->field_14 = v9;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = v6;
            if (!((char)((((unsigned short)v12 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                v13 = v12 - 1;
                if (/* unsupported instruction */)
                {
                    v14 = v13 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v14 = v13 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                v12 = v14 + 1;
                if (!((char)((((unsigned short)v12 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v10)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    v16 = v12 - 1;
                    if (/* unsupported instruction */)
                    {
                        v17 = v16 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v17 = v16 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    if (/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    idx->field_18 = v9 + 1;
                    if (/* unsupported instruction */)
                    {
                        idx->field_1c = /* unsupported instruction */;
                        /* unsupported instruction */
                        v6 = v17 + 1;
                    }
                    else
                    {
                        idx->field_1c = nan;
                        /* unsupported instruction */
                        v6 = v17 + 1;
                    }
                    goto LABEL_428380;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v6 = v12 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_1c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            idx->field_18 = sub_4931e0();
LABEL_428380:
            idx->field_7d = 1;
        }
    } while ((idx = (st_428270_5 *)v8, idx));
    return 1;
}
