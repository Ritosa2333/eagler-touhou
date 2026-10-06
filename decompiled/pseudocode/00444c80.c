// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x444c80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_444c80_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    int field_28;
    int field_2c;
    unsigned int field_30;
    char padding_34[196];
    unsigned int field_f8;
    char padding_fc[444];
    int field_2b8;
    char padding_2bc[452];
    unsigned int field_480;
    char padding_484[488];
    int field_66c;
    char padding_670[21276];
    unsigned int field_598c;
} st_444c80_0;

typedef struct st_444c80_3 {
    char padding_0[16];
    struct st_444c80_2 *field_10;
    struct st_444c80_3 *field_14;
} st_444c80_3;

typedef struct st_444c80_2 {
    unsigned int field_0;
    char padding_4[998];
    unsigned short field_3ea;
} st_444c80_2;

typedef struct st_444c80_1 {
    char padding_0[1432];
    unsigned int field_598;
    char padding_59c[17904];
    unsigned int field_4b8c;
} st_444c80_1;

extern int g_4aebd0;
extern unsigned int g_4b0c90;
extern int g_4b0ca8;
extern void* g_4b43b8;
extern st_444c80_1 *g_4b451c;
extern unsigned int g_4ce8bc;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_444c80(st_444c80_0 *idx)
{
    unsigned int v6;  // esi
    unsigned int v7;  // eax
    st_444c80_3 *iter;  // eax
    st_444c80_3 *v16;  // eax
    st_444c80_3 *iter1;  // eax
    st_444c80_2 *v18;  // ecx
    st_444c80_3 *v19;  // eax
    st_444c80_3 *node;  // eax
    st_444c80_0 *v21;  // edi
    int v8;  // ebx
    void* v22;  // esi
    int v23;  // eax
    st_444c80_3 *v24;  // eax
    st_444c80_3 *iter2;  // eax
    int v26;  // eax
    unsigned int v27;  // edi
    int v9;  // ecx
    st_444c80_3 *v10;  // eax
    st_444c80_3 *v11;  // eax
    st_444c80_3 *v12;  // eax
    st_444c80_3 *v13;  // eax
    st_444c80_3 *v14;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    int v2;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x18]
    int v4;  // [bp-0x14]
    int v5;  // [bp+0x0]

    v6 = (unsigned int)((g_4b0ca8 >= 4) + 131);
    switch (idx->field_24)
    {
    case 0:
        if (!idx->field_66c)
        {
            v7 = (int)g_4b43b8[102324];
            v3 = 20;
            v2 = &idx;
            sub_4615a0(v7, &idx, 20, 0);
            idx->field_66c = v8;
        }
        idx->field_30 = ((unsigned int)((g_4aebd0 >= 4) + 1) & 3) + 1;
        g_4b0ca8 = g_4aebd0;
        sub_461970(*((int *)&idx->padding_2bc[12 + 4 * v6]), v7);
        *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
        sub_43efa0(v2);
        sub_4619e0(*((int *)&idx->padding_2bc[12 + 4 * v6]));
        sub_461970(*((int *)&idx->padding_2bc[12 + 4 * v6]), *((int *)&idx->padding_2bc[12 + 4 * v6]));
        v9 = idx->field_14;
        sub_4615a0(v9, &esi, 110, 0);
        idx->field_480 = v3;
        sub_43ef40(v9, &esi, 110, 0);
        if (g_4b0ca8 < 4)
        {
            if (!g_4b451c->field_598 || !g_4b451c->field_4b8c || !*((int *)&g_4b451c[1].padding_59c[16468]) || !*((int *)&g_4b451c[2].padding_59c[15032]) || !*((int *)&g_4b451c[3].padding_59c[13596]) || !*((int *)&g_4b451c[4].padding_59c[12160]))
            {
                v10 = sub_461920(*((int *)&idx->padding_2bc[12 + 4 * v6]));
                if (!v10)
                    *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
                v11 = &v10->field_10;
                if (v11)
                {
                    do
                    {
                        if (*((short *)(*((int *)&v11->padding_0[0]) + 1002)) == 0xaa)
                        {
                            v2 = *((int *)*((int *)&v11->padding_0[0]));
                            goto LABEL_444e0f;
                        }
                    } while ((v11 = (st_444c80_3 *)*((int *)&v11->padding_0[4]), v11));
                }
                v2 = 0;
LABEL_444e0f:
                sub_461d80();
            }
            if (!*((int *)&g_4b451c->padding_59c[0]) || !*((int *)&g_4b451c[1].padding_0[0]) || !*((int *)&g_4b451c[1].padding_59c[16472]) || !*((int *)&g_4b451c[2].padding_59c[15036]) || !*((int *)&g_4b451c[3].padding_59c[13600]) || !*((int *)&g_4b451c[4].padding_59c[12164]))
            {
                v12 = sub_461920(*((int *)&idx->padding_2bc[12 + 4 * v6]));
                if (!v12)
                    *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
                v13 = &v12->field_10;
                if (v13)
                {
                    do
                    {
                        if (*((short *)(*((int *)&v13->padding_0[0]) + 1002)) == 171)
                        {
                            v7 = *((int *)*((int *)&v13->padding_0[0]));
                            goto LABEL_444e8f;
                        }
                    } while ((v13 = (st_444c80_3 *)*((int *)&v13->padding_0[4]), v13));
                }
                v7 = 0;
LABEL_444e8f:
                sub_461d80();
            }
            if (!*((int *)&g_4b451c->padding_59c[4]) || !*((int *)&g_4b451c[1].padding_0[4]) || !*((int *)&g_4b451c[1].padding_59c[16476]) || !*((int *)&g_4b451c[2].padding_59c[15040]) || !*((int *)&g_4b451c[3].padding_59c[13604]) || !*((int *)&g_4b451c[4].padding_59c[12168]))
            {
                v14 = sub_461920(*((int *)&idx->padding_2bc[12 + 4 * v6]));
                if (!v14)
                    *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
                iter = &v14->field_10;
                if (iter)
                {
                    do
                    {
                        if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == 172)
                        {
                            v1 = *((int *)*((int *)&iter->padding_0[0]));
                            goto LABEL_444f0f;
                        }
                    } while ((iter = (st_444c80_3 *)*((int *)&iter->padding_0[4]), iter));
                }
                v1 = 0;
LABEL_444f0f:
                sub_461d80();
            }
            if (*((int *)&g_4b451c->padding_59c[8]) && *((int *)&g_4b451c[1].padding_0[8]) && *((int *)&g_4b451c[1].padding_59c[16480]) && *((int *)&g_4b451c[2].padding_59c[15044]) && *((int *)&g_4b451c[3].padding_59c[13608]))
            {
                if (*((int *)&g_4b451c[4].padding_59c[12172]))
                    goto LABEL_445054;
                goto LABEL_444f4e;
            }
            else
            {
LABEL_444f4e:
                v16 = sub_461920(*((int *)&idx->padding_2bc[12 + 4 * v6]));
                if (!v16)
                    *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
                iter1 = &v16->field_10;
                if (iter1)
                {
                    do
                    {
                        v18 = *((int *)&iter1->padding_0[0]);
                        if (v18->field_3ea == 173)
                            goto LABEL_445079;
                    } while ((iter1 = (st_444c80_3 *)*((int *)&iter1->padding_0[4]), iter1));
                }
                v0 = 0;
LABEL_44504b:
                sub_461d80();
            }
        }
        else
        {
            if (*((int *)&g_4b451c->padding_59c[12]) && *((int *)&g_4b451c[1].padding_0[12]) && *((int *)&g_4b451c[1].padding_59c[16484]) && *((int *)&g_4b451c[2].padding_59c[15048]) && *((int *)&g_4b451c[3].padding_59c[13612]) && *((int *)&g_4b451c[4].padding_59c[12176]))
                goto LABEL_445054;
            v19 = sub_461920(*((int *)&idx->padding_2bc[12 + 4 * v6]));
            if (!v19)
                *((unsigned int *)&idx->padding_2bc[12 + 4 * v6]) = 0;
            node = &v19->field_10;
            if (node)
            {
                while (1)
                {
                    v18 = *((int *)&node->padding_0[0]);
                    if (v18->field_3ea == 174)
                        break;
                    node = *((int *)&node->padding_0[4]);
                    if (!node)
                        goto LABEL_445047;
                }
LABEL_445079:
                v0 = v18->field_0;
                goto LABEL_44504b;
            }
            else
            {
LABEL_445047:
                v2 = 0;
                goto LABEL_44504b;
            }
        }
    case 1:
LABEL_445054:
        if (idx->field_2b8 <= 6)
            return 1;
        sub_43ef40();
        return 1;
    case 2:
        if (g_4b0ca8 < 4)
        {
            v21 = &idx->field_28;
            *((int *)&v21->padding_0[4]) = idx->field_28;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
            {
                v4 = -0x1;
                sub_464970(-0x1);
            }
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
            {
                v3 = 1;
                sub_464970(1);
            }
            if (*((int *)&v21->padding_0[4]) != *((int *)&v21->padding_0[0]))
            {
                sub_453d90();
                sub_43f010();
                sub_461970(*((int *)&idx->padding_2bc[12 + 4 * v6]), v3);
            }
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            sub_43efd0();
            return 1;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            v22 = &idx->padding_0[4 * v6 + 712];
            sub_461970(*((int *)&idx->padding_2bc[12 + 4 * v6]), v3);
            if (g_4b0ca8 < 4)
            {
                sub_462060();
                v23 = v5;
                break;
            }
            else
            {
                v3 = *((int *)v22);
                v24 = sub_461920(*((int *)v22));
                if (!v24)
                    *((unsigned int *)v22) = 0;
                iter2 = &v24->field_10;
                if (v24 != 0xfffffff0)
                {
                    do
                    {
                        if (*((short *)(*((int *)&iter2->padding_0[0]) + 1002)) == 125)
                        {
                            v23 = *((int *)*((int *)&iter2->padding_0[0]));
                            break;
                        }
                    } while ((iter2 = (st_444c80_3 *)*((int *)&iter2->padding_0[4]), iter2));
                }
                v23 = 0;
                break;
            }
            sub_461970(v23, v3);
            sub_43ef40(v4);
            sub_453d90();
            return 1;
        }
        goto LABEL_4452a4;
    case 3:
        if (idx->field_2b8 >= 14)
        {
            sub_43efd0();
            sub_43eee0();
            v26 = g_4b0ca8;
            if (v26 < 4)
            {
                v26 = idx->field_28;
                g_4b0ca8 = v26;
            }
            g_4aebd0 = v26;
            sub_464900();
            idx->field_f8 = 1;
            idx->field_30 = 3;
            sub_40f790();
            g_4b0c90 = g_4ce8bc;
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
            sub_461970(idx->field_66c, v27);
            idx->field_66c = 0;
            sub_43eee0(v8);
            g_4b0ca8 = (g_4b0ca8 < 4 ? idx->field_28 : idx->field_598c);
            g_4aebd0 = g_4b0ca8;
            sub_464940();
        }
LABEL_4452a4:
        return 1;
    default:
        return 1;
    }
}
