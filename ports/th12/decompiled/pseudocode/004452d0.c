// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4452d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4452d0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    int field_30;
    char padding_34[200];
    unsigned int field_fc;
    char padding_100[440];
    int field_2b8;
    char padding_2bc[456];
    unsigned int field_484;
    char padding_488[124];
    int field_504;
} st_4452d0_0;

typedef struct st_4452d0_3 {
    char padding_0[16];
    struct st_4452d0_2 *field_10;
    struct st_4452d0_3 *field_14;
} st_4452d0_3;

typedef struct st_4452d0_2 {
    unsigned int field_0;
    char padding_4[998];
    unsigned short field_3ea;
} st_4452d0_2;

typedef struct st_4452d0_1 {
    char padding_0[125392];
    char field_1e9d0;
    char field_1e9d1;
    char field_1e9d2;
    char field_1e9d3;
    char field_1e9d4;
    char field_1e9d5;
} st_4452d0_1;

extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern st_4452d0_1 *g_4b451c;
extern unsigned int g_4ce8bc;
extern unsigned int g_4ce8c0;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_4452d0(unsigned int a0, unsigned int a1, st_4452d0_0 *idx)
{
    int v5;  // eax
    st_4452d0_3 *v14;  // eax
    st_4452d0_3 *iter;  // eax
    st_4452d0_3 *v16;  // eax
    st_4452d0_3 *node;  // eax
    st_4452d0_3 *v18;  // eax
    st_4452d0_3 *iter1;  // eax
    st_4452d0_0 *v20;  // esi
    unsigned int v21;  // ecx
    st_4452d0_0 *v22;  // esi
    int v23;  // esi
    int v6;  // eax
    int v24;  // edi
    st_4452d0_3 *v25;  // eax
    st_4452d0_3 *iter2;  // eax
    int v27;  // eax
    st_4452d0_3 *v28;  // eax
    st_4452d0_3 *v29;  // eax
    int v30;  // eax
    st_4452d0_3 *v31;  // eax
    st_4452d0_3 *v32;  // eax
    int v33;  // eax
    int v7;  // eax
    st_4452d0_3 *v34;  // eax
    st_4452d0_3 *v35;  // eax
    int v36;  // eax
    st_4452d0_3 *v37;  // eax
    st_4452d0_3 *v38;  // eax
    int v39;  // eax
    unsigned int v40;  // ecx
    unsigned int v41;  // edi
    unsigned int *v42;  // eax
    unsigned int v43;  // ecx
    int v8;  // eax
    unsigned int v44;  // 4101
    int v9;  // eax
    int v10;  // eax
    int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x2c]

    switch (idx->field_24)
    {
    case 2:
        v20 = &idx->field_28;
        *((unsigned int *)&v20->padding_0[4]) = idx->field_28;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (*((int *)&v20->padding_0[4]) != *((int *)&v20->padding_0[0]))
        {
            sub_453d90(a0, 10);
            sub_4619e0(idx->field_504);
            sub_461970(idx->field_504);
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90(v21, 9);
            return 1;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            v22 = &idx->field_504;
            sub_462060();
            sub_461970(v23);
            sub_462060();
            sub_461970(v24);
            v25 = sub_461920(*((int *)&v22->padding_0[0]));
            if (!v25)
                *((int *)&v22->padding_0[0]) = 0;
            iter2 = &v25->field_10;
            if (v25 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&iter2->padding_0[0]) + 1002)) == 95)
                    {
                        v27 = *((int *)*((int *)&iter2->padding_0[0]));
                        goto LABEL_445768;
                    }
                } while ((iter2 = (st_4452d0_3 *)*((int *)&iter2->padding_0[4]), iter2));
            }
            v27 = 0;
LABEL_445768:
            sub_461970(v27);
            v28 = sub_461920(*((int *)&v22->padding_0[0]));
            if (!v28)
                *((int *)&v22->padding_0[0]) = 0;
            v29 = &v28->field_10;
            if (v28 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&v29->padding_0[0]) + 1002)) == 96)
                    {
                        v30 = *((int *)*((int *)&v29->padding_0[0]));
                        goto LABEL_4457a8;
                    }
                } while ((v29 = (st_4452d0_3 *)*((int *)&v29->padding_0[4]), v29));
            }
            v30 = 0;
LABEL_4457a8:
            sub_461970(v30);
            sub_462060();
            sub_461970(v23);
            sub_462060();
            sub_461970(v24);
            v31 = sub_461920(*((int *)&v22->padding_0[0]));
            if (!v31)
                *((int *)&v22->padding_0[0]) = 0;
            v32 = &v31->field_10;
            if (v31 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&v32->padding_0[0]) + 1002)) == 0x88)
                    {
                        v33 = *((int *)*((int *)&v32->padding_0[0]));
                        goto LABEL_44583e;
                    }
                } while ((v32 = (st_4452d0_3 *)*((int *)&v32->padding_0[4]), v32));
            }
            v33 = 0;
LABEL_44583e:
            sub_461970(v33);
            v34 = sub_461920(*((int *)&v22->padding_0[0]));
            if (!v34)
                *((int *)&v22->padding_0[0]) = 0;
            v35 = &v34->field_10;
            if (v34 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&v35->padding_0[0]) + 1002)) == 137)
                    {
                        v36 = *((int *)*((int *)&v35->padding_0[0]));
                        goto LABEL_44587d;
                    }
                } while ((v35 = (st_4452d0_3 *)*((int *)&v35->padding_0[4]), v35));
            }
            v36 = 0;
LABEL_44587d:
            sub_461970(v36);
            v37 = sub_461920(*((int *)&v22->padding_0[0]));
            if (!v37)
                *((int *)&v22->padding_0[0]) = 0;
            v38 = &v37->field_10;
            if (v37 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&v38->padding_0[0]) + 1002)) == 138)
                    {
                        v39 = *((int *)*((int *)&v38->padding_0[0]));
                        goto LABEL_4458b9;
                    }
                } while ((v38 = (st_4452d0_3 *)*((int *)&v38->padding_0[4]), v38));
            }
            v39 = 0;
LABEL_4458b9:
            sub_461970(v39);
            sub_453d90(v40, 7);
            sub_43ef40();
            return 1;
        }
        goto LABEL_445a11;
    case 3:
        if (idx->field_2b8 >= 14)
        {
            sub_43efd0();
            sub_43eee0();
            v41 = g_4b0c90;
            g_4b0c90 = idx->field_28;
            g_4ce8bc = g_4b0c90;
            v42 = sub_464900();
            idx->field_30 = 3;
            if (v41 == g_4b0c90)
            {
                sub_40f790();
                g_4b0c94 = g_4ce8c0;
                return 1;
            }
            v43 = v42[2];
            if (!v43)
            {
                *(v42) = 0;
                g_4ce8c0 = 0;
                return 1;
            }
            v44 = _ccall(14, 15, v43, 0, 0);
            if (v44 & 1)
            {
                *(v42) = v43 - 1;
                g_4ce8c0 = 0;
            }
            else
            {
                *(v42) = 0;
                g_4ce8c0 = 0;
            }
            return 1;
        }
        else
        {
            return 1;
        }
    case 4:
        if (idx->field_2b8 >= 6)
        {
            sub_43efd0();
            sub_43efd0();
            sub_43eee0();
            g_4b0c90 = idx->field_28;
            sub_464940();
            g_4ce8bc = g_4b0c90;
        }
LABEL_445a11:
        return 1;
    case 0:
        idx->field_30 = 3;
        if (g_4b0ca8 == 4)
        {
            if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16))
            {
                if (!idx->field_28)
                {
                    v5 = idx->field_30;
                    if (!v5)
                    {
                        idx->field_28 = 1;
                    }
                    else if (v5 <= 1)
                    {
                        idx->field_28 = v5 - 1;
                    }
                    else
                    {
                        idx->field_28 = 1;
                    }
                }
                *((unsigned int *)&idx->padding_34[132 + 4 * idx->field_fc]) = 0;
                idx->field_fc = idx->field_fc + 1;
            }
            if (!(g_4b451c->field_1e9d2 & 16) && !(g_4b451c->field_1e9d3 & 16))
            {
                if (idx->field_28 == 1)
                {
                    if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16))
                    {
                        v6 = idx->field_30;
                        if (!v6)
                        {
                            idx->field_28 = 2;
                            break;
                        }
                        else if (v6 <= 2)
                        {
                            idx->field_28 = v6 - 1;
                            break;
                        }
                        else
                        {
                            idx->field_28 = 2;
                            break;
                        }
                    }
                    else
                    {
                        v7 = idx->field_30;
                        if (!v7)
                        {
                            idx->field_28 = 0;
                            break;
                        }
                        else if (v7 <= 0)
                        {
                            idx->field_28 = v7 - 1;
                            break;
                        }
                        else
                        {
                            idx->field_28 = 0;
                            break;
                        }
                    }
                }
                *((unsigned int *)&idx->padding_34[132 + 4 * idx->field_fc]) = 1;
                idx->field_fc = idx->field_fc + 1;
            }
            if (!(g_4b451c->field_1e9d4 & 16) && !(g_4b451c->field_1e9d5 & 16))
            {
                if (idx->field_28 == 2)
                {
                    if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16))
                    {
                        v8 = idx->field_30;
                        if (!v8)
                        {
                            idx->field_28 = 1;
                        }
                        else if (v8 <= 1)
                        {
                            idx->field_28 = v8 - 1;
                        }
                        else
                        {
                            idx->field_28 = 1;
                        }
                    }
                    else
                    {
                        v9 = idx->field_30;
                        if (!v9)
                        {
                            idx->field_28 = 0;
                        }
                        else if (v9 <= 0)
                        {
                            idx->field_28 = v9 - 1;
                        }
                        else
                        {
                            idx->field_28 = 0;
                        }
                    }
                }
                *((unsigned int *)&idx->padding_34[132 + 4 * idx->field_fc]) = 2;
                idx->field_fc = idx->field_fc + 1;
            }
        }
        v10 = idx->field_14;
        sub_4615a0(v10, &idx, 111, 0);
        idx->field_484 = v23;
        sub_461970(idx->field_504);
        idx->field_504 = 0;
        sub_4615a0(idx->field_14, &v24, 143, 0);
        v12 = v10;
        idx->field_504 = v12;
        sub_4619e0(v12);
        sub_461970(idx->field_504);
        sub_43ef40(idx->field_504);
        v13 = g_4b0ca8;
        if (!*((int *)&g_4b451c->padding_0[1432 + 4 * g_4b0ca8]) || !*((int *)&g_4b451c->padding_0[19340 + 4 * g_4b0ca8]))
        {
            v14 = sub_461920(idx->field_504);
            if (!v14)
                idx->field_504 = 0;
            iter = &v14->field_10;
            if (iter)
            {
                do
                {
                    if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == 0x88)
                    {
                        v2 = *((int *)*((int *)&iter->padding_0[0]));
                        goto LABEL_44552f;
                    }
                } while ((iter = (st_4452d0_3 *)*((int *)&iter->padding_0[4]), iter));
            }
            v2 = 0;
LABEL_44552f:
            sub_461d80();
            v13 = g_4b0ca8;
        }
        if (!*((int *)&g_4b451c->padding_0[37248 + 4 * v13]) || !*((int *)&g_4b451c->padding_0[55156 + 4 * v13]))
        {
            v16 = sub_461920(idx->field_504);
            if (!v16)
                idx->field_504 = 0;
            node = &v16->field_10;
            if (node)
            {
                do
                {
                    if (*((short *)(*((int *)&node->padding_0[0]) + 1002)) == 137)
                    {
                        v1 = *((int *)*((int *)&node->padding_0[0]));
                        goto LABEL_44558f;
                    }
                } while ((node = (st_4452d0_3 *)*((int *)&node->padding_0[4]), node));
            }
            v1 = 0;
LABEL_44558f:
            sub_461d80();
            v13 = g_4b0ca8;
        }
        if (!*((int *)&g_4b451c->padding_0[73064 + 4 * v13]) || !*((int *)&g_4b451c->padding_0[90972 + 4 * v13]))
        {
            v18 = sub_461920(idx->field_504);
            if (!v18)
                idx->field_504 = 0;
            iter1 = &v18->field_10;
            if (iter1)
            {
                do
                {
                    if (*((short *)(*((int *)&iter1->padding_0[0]) + 1002)) == 138)
                    {
                        v0 = *((int *)*((int *)&iter1->padding_0[0]));
                        goto LABEL_4455eb;
                    }
                } while ((iter1 = (st_4452d0_3 *)*((int *)&iter1->padding_0[4]), iter1));
            }
            v0 = 0;
LABEL_4455eb:
            sub_461d80();
        }
    case 1:
        if (idx->field_2b8 <= 6)
            return 1;
        sub_43ef40();
        return 1;
    default:
        return 1;
    }
}
