// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44a2d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44a2d0_9 {
    char padding_0[16];
    struct st_44a2d0_8 *field_10;
    struct st_44a2d0_9 *field_14;
} st_44a2d0_9;

typedef struct st_44a2d0_2 {
    unsigned int field_0;
    struct st_44a2d0_2 *field_4;
} st_44a2d0_2;

typedef struct st_44a2d0_0 {
    char padding_0[4732];
    unsigned int field_127c;
    char padding_1280[5064];
    char field_2648;
    char padding_2649[19];
    unsigned int field_265c;
} st_44a2d0_0;

typedef struct st_44a2d0_1 {
    char padding_0[20];
    int field_14;
    char padding_18[16];
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[4];
    struct st_44a2d0_0 *field_34;
    unsigned int field_38;
    unsigned int field_3c;
    int field_40;
    int field_44;
    char padding_48[28];
    int field_64;
} st_44a2d0_1;

typedef struct st_44a2d0_8 {
    char padding_0[36];
    unsigned int field_24;
    char padding_28[962];
    unsigned short field_3ea;
    char padding_3ec[144];
    unsigned int field_47c;
} st_44a2d0_8;

typedef struct st_44a2d0_6 {
    unsigned int field_0;
} st_44a2d0_6;

typedef struct st_44a2d0_5 {
    char padding_0[64];
    struct st_44a2d0_6 *field_40;
} st_44a2d0_5;

typedef struct st_44a2d0_7 {
    struct st_44a2d0_5 *field_0;
} st_44a2d0_7;

typedef struct st_44a2d0_4 {
    char padding_0[27952];
    unsigned int field_6d30;
} st_44a2d0_4;

typedef struct st_44a2d0_3 {
    char padding_0[104];
    struct st_44a2d0_2 *field_68;
} st_44a2d0_3;

extern unsigned int g_4b0c5c;
extern int g_4b0c60;
extern st_44a2d0_3 *g_4b43dc;
extern st_44a2d0_4 *g_4b43e4;
extern st_44a2d0_7 *g_4d1380;

int sub_44a2d0(void)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int *index;  // eax
    st_44a2d0_9 *v16;  // eax
    st_44a2d0_9 *iter;  // eax
    st_44a2d0_8 *idx;  // eax
    st_44a2d0_1 *idx1;  // eax
    unsigned int v8;  // eax
    st_44a2d0_2 *v9;  // eax
    st_44a2d0_2 *v10;  // eax
    unsigned int idx2;  // eax
    int v12;  // edi
    int v13;  // edi
    unsigned int v14;  // eax
    unsigned int v0;  // [bp-0x24]
    int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v3 = v5;
    v2 = v6;
    if (idx1->field_38)
    {
        v8 = idx1->field_14 & 0x8000000f;
        if ((idx1->field_14 & 0x8000000f) < 0)
            v8 = (v8 - 1 | 0xfffffff0) + 1;
        if (v8 == 1)
        {
            v1 = 1;
        }
        else
        {
            if (v8 != 9)
                goto LABEL_44a302;
            v1 = 0;
        }
        sub_421690(v1);
LABEL_44a302:
        if (idx1->field_38)
        {
            v9 = g_4b43dc->field_68;
            if (g_4b43dc->field_68)
            {
                do
                {
                    v10 = v9;
                    if (*((int *)(v10->field_0 + 10164)) == idx1->field_38)
                    {
                        if (g_4b43e4->field_6d30)
                        {
                            idx2 = &idx1->field_34->field_2648;
                            *((unsigned int *)(idx2 + 20)) = *((int *)(idx2 + 20)) | 2;
                        }
                        v12 = idx1->field_28 - 300;
                        if (!(idx1->field_14 != v12 && !sub_449f20(v13)))
                        {
                            idx1->field_34->field_127c = 999;
                        }
                        else if (idx1->field_14 < v12)
                        {
                            idx1->field_34->field_127c = 0xfffffc19;
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
                        v14 = sub_4931e0();
                        g_4d1380->field_0->field_40(g_4d1380, v14);
                        index = sub_461920(idx1->field_3c);
                        if (!index)
                            idx1->field_3c = 0;
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
                        if (/* unsupported instruction */)
                        {
                            index[268] = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            index[268] = nan;
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
                        if (/* unsupported instruction */)
                        {
                            index[269] = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            index[269] = nan;
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
                        if (/* unsupported instruction */)
                        {
                            index[270] = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            index[270] = nan;
                            /* unsupported instruction */
                        }
                        v0 = idx1->field_3c;
                        v16 = sub_461920();
                        if (!v16)
                            idx1->field_3c = 0;
                        iter = &v16->field_10;
                        if (iter)
                        {
                            do
                            {
                                if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == 206)
                                {
                                    idx = *((int *)&iter->padding_0[0]);
                                    goto LABEL_44a499;
                                }
                            } while ((iter = (st_44a2d0_9 *)*((int *)&iter->padding_0[4]), iter));
                        }
                        idx = NULL;
LABEL_44a499:
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
                        idx->field_47c = idx->field_47c | 4;
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
                        if (/* unsupported instruction */)
                        {
                            idx->field_24 = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            idx->field_24 = nan;
                            /* unsupported instruction */
                        }
                        goto LABEL_44a4cd;
                    }
                } while ((v9 = (st_44a2d0_2 *)v10->field_4, v10->field_4));
            }
        }
        memset(&idx1->field_34, 0, 8);
        idx1->field_2c = 0;
        sub_453f10();
        sub_4214b0(g_4b43e4, -0x1);
        sub_461a70(idx1->field_3c);
        idx1->field_3c = 0;
        sub_461970(idx1->field_40);
        sub_461970(idx1->field_44);
        sub_464a80(idx1->field_44);
        return;
    }
    else
    {
        if (g_4b0c5c == 3 && g_4b0c60 >= 0)
            sub_44a8a0();
LABEL_44a4cd:
        if (idx1->field_64 > 0)
            idx1->field_64 = idx1->field_64 - 1;
        sub_464a80();
        return;
    }
}
