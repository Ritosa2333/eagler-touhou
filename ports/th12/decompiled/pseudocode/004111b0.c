// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4111b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4111b0_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[4];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char padding_24[4];
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[4];
    unsigned int field_3c;
    char padding_40[20];
    int field_54;
    char padding_58[28];
    unsigned int field_74;
    char padding_78[4];
    unsigned int field_7c;
} st_4111b0_0;

typedef struct st_4111b0_6 {
    struct st_4111b0_5 *field_0;
    struct st_4111b0_6 *field_4;
} st_4111b0_6;

typedef struct st_4111b0_1 {
    unsigned int field_0;
    struct st_4111b0_1 *field_4;
} st_4111b0_1;

typedef struct st_4111b0_5 {
    char padding_0[1152];
    unsigned int field_480;
} st_4111b0_5;

extern char g_4a3738;
extern unsigned int g_4ad138;
extern char g_4b2ed0;
extern unsigned int g_4ce8cc;
extern int g_4cee70;

st_4111b0_0 * sub_4111b0(st_4111b0_0 *a0)
{
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    st_4111b0_0 *i;  // eax
    st_4111b0_0 *iter;  // edi
    unsigned int v20;  // eax
    unsigned int v21;  // ecx
    st_4111b0_1 *iter2;  // eax
    unsigned int v23;  // ebx
    st_4111b0_1 *v24;  // eax
    unsigned short v7;  // fs
    unsigned int v25;  // eax
    unsigned int j;  // ecx
    st_4111b0_6 *v27;  // eax
    st_4111b0_5 *index;  // eax
    st_4111b0_6 *v29;  // eax
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    unsigned int v32;  // eax
    unsigned long long v33;  // 4140
    unsigned long long v8;  // 4165
    unsigned long long v9;  // 4187
    st_4111b0_0 *idx;  // esi
    int v11;  // edi
    int v12;  // esi
    int v13;  // ebp
    int v14;  // ebx
    st_4111b0_0 *v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = 0xffffffff;
    v3 = sub_4974de;
    v8 = _ccall(v5, v6, v7, 0);
    v2 = *((int *)(unsigned int)v8);
    v9 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v9) = &v2;
    idx = a0;
    idx->field_14 = idx->field_14 & 0xfffffffe;
    idx->field_28 = idx->field_28 & 0xfffffffe;
    idx->field_3c = idx->field_3c & 0xfffffffe;
    *((char **)&idx[1].padding_40[16]) = &g_4a3738;
    memset(&idx[1].field_54, 0, 16);
    v4 = 0;
    sub_477420(idx, 0, 240, g_4ad138 ^ &v11, v11, v12, v13, v14);
    i = NULL;
    v1 = 0;
    iter = idx->padding_40;
    v0 = idx;
    do
    {
        sub_4615a0(g_4cee70, &v12, &i->padding_40[4], 0);
        *((unsigned int *)&iter->padding_0[0]) = 0;
        v20 = 0;
        *((unsigned int *)&iter->padding_0[0]) = 0;
        *((char *)(v20 + 1180)) = 16;
        v21 = (unsigned int)iter->padding_0;
        if (!v21)
        {
LABEL_4112ad:
            v25 = 0;
            goto LABEL_4112ee;
        }
        else
        {
            iter2 = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    v23 = iter2->field_0;
                    if (*((int *)v23) == v21)
                        goto LABEL_4112e8;
                } while ((iter2 = (st_4111b0_1 *)iter2->field_4, iter2));
            }
            v24 = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_4112ad;
            while (1)
            {
                v23 = v24->field_0;
                if (*((int *)v23) == v21)
                    break;
                v24 = v24->field_4;
                if (!v24)
                {
                    v25 = 0;
                    goto LABEL_4112ee;
                }
            }
LABEL_4112e8:
            v25 = v23;
            if (v25)
                goto LABEL_4112f0;
LABEL_4112ee:
            *((unsigned int *)&iter->padding_0[0]) = 0;
LABEL_4112f0:
            *((char *)(v25 + 1181)) = 16;
            j = (unsigned int)iter->padding_0;
            if (!j)
            {
LABEL_4112fd:
                index = NULL;
                goto LABEL_411342;
            }
            else
            {
                v27 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        if (*((int *)&v27->field_0->padding_0[0]) == j)
                        {
                            index = v27->field_0;
                            goto LABEL_41133e;
                        }
                    } while ((v27 = (st_4111b0_6 *)v27->field_4, v27));
                }
                v29 = *((int *)(g_4ce8cc + 8935104));
                if (!*((int *)(g_4ce8cc + 8935104)))
                    goto LABEL_4112fd;
                while (*((int *)&v29->field_0->padding_0[0]) != j)
                {
                    if (!v29->field_4)
                    {
                        index = NULL;
                        goto LABEL_411342;
                    }
                }
                index = v29->field_0;
LABEL_41133e:
                if (index)
                    goto LABEL_411344;
LABEL_411342:
                *((unsigned int *)&iter->padding_0[0]) = 0;
            }
        }
LABEL_411344:
        index->field_480 = index->field_480 | 8;
        v0 = &v0->padding_0[1];
        iter = &iter->field_4;
        i = v0;
    } while (i < 0x5);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_54 = v14;
    v30 = idx->field_14;
    if (!((char)idx->field_14 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_c = /* unsupported instruction */;
        else
            idx->field_c = nan;
        idx->field_8 = 0;
        idx->field_4 = 0xfff0bdc1;
        *((char **)&idx->padding_10[0]) = &g_4b2ed0;
        idx->field_14 = v30 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_c = /* unsupported instruction */;
    else
        idx->field_c = nan;
    idx->field_8 = 0;
    idx->field_4 = 0xffffffff;
    v31 = idx->field_28;
    if (!((char)idx->field_28 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_20 = /* unsupported instruction */;
        else
            idx->field_20 = nan;
        idx->field_1c = 0;
        idx->field_18 = 0xfff0bdc1;
        *((char **)&idx->padding_24[0]) = &g_4b2ed0;
        idx->field_28 = v31 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_20 = /* unsupported instruction */;
    else
        idx->field_20 = nan;
    idx->field_1c = 0;
    idx->field_18 = 0xffffffff;
    v32 = idx->field_3c;
    if (!((char)idx->field_3c & 1))
    {
        if (/* unsupported instruction */)
            idx->field_34 = /* unsupported instruction */;
        else
            idx->field_34 = nan;
        idx->field_30 = 0;
        idx->field_2c = 0xfff0bdc1;
        *((char **)&idx->padding_38[0]) = &g_4b2ed0;
        idx->field_3c = v32 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_34 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_34 = nan;
        /* unsupported instruction */
    }
    idx->field_30 = 0;
    idx->field_2c = 0xffffffff;
    idx->field_74 = idx->field_74 | 1;
    idx->field_7c = 0xffffff;
    v33 = _ccall(v5, v6, v7, 0);
    *((unsigned int *)(unsigned int)v33) = 240;
    return idx;
}
