// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410170; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410170_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char field_c;
} st_410170_1;

typedef struct st_410170_2 {
    int field_0;
    int field_4;
    char padding_8[8];
    struct st_410170_0 *field_10;
    struct st_410170_1 *field_14;
} st_410170_2;

typedef struct st_410170_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[8];
    unsigned int field_14;
    unsigned int field_18;
    char field_1c;
} st_410170_0;

int sub_410170(void)
{
    st_410170_2 *index;  // edi
    int v3;  // eax
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    int i;  // esi
    st_410170_0 *iter;  // ecx
    st_410170_1 *idx;  // edx
    unsigned int v6;  // ebx
    int v7;  // ebx
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v0;  // [bp-0x4]

    v3 = index->field_0;
    if (!v3)
        return v3;
    iter = index->field_10;
    idx = index->field_14;
    v0 = v6;
    v7 = 0;
    if (v3 > 0)
    {
        v9 = v8 - 1;
        if (/* unsupported instruction */)
        {
            v10 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v10 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v11 = v10 - 1;
        if (/* unsupported instruction */)
        {
            v12 = v11 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v12 = v11 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = v12 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        do
        {
            i = 0;
            if (index->field_4 > 0)
            {
                do
                {
                    iter->field_0 = idx->field_0;
                    iter->field_4 = idx->field_4;
                    iter->field_8 = idx->field_8;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter->field_14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter->field_18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    if (!((char)((((unsigned short)v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), iter->field_14) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        if (/* unsupported instruction */)
                            iter->field_14 = /* unsupported instruction */;
                        else
                            iter->field_14 = nan;
                    }
                    if (!((char)((((unsigned short)v13 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, iter->field_18) * 0x100 & 0x4500 : CmpF(nan, iter->field_18) * 0x100 & 0x4500) & 0x4700) >> 8) & 65))
                    {
                        if (/* unsupported instruction */)
                            iter->field_18 = /* unsupported instruction */;
                        else
                            iter->field_18 = nan;
                    }
                    i += 1;
                    idx = &idx->field_c;
                    iter = &iter->field_1c;
                } while (i < index->field_4);
            }
        } while ((v7 = (int)(v7 + 1), v7 < index->field_0));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    return v3;
}
