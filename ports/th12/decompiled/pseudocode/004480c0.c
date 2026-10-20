// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4480c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4480c0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[4];
    int field_30;
    char padding_34[196];
    unsigned int field_f8;
    char padding_fc[444];
    int field_2b8;
    char padding_2bc[408];
    int field_454;
    char padding_458[64];
    unsigned int field_498;
    char padding_49c[464];
    unsigned int field_66c;
    char padding_670[21256];
    char field_5978;
    char field_5979;
    char padding_597a[10];
    int field_5984;
    unsigned int field_5988;
    char padding_598c[4];
    unsigned int field_5990;
    unsigned int field_5994;
    unsigned int field_5998;
    char padding_599c[196];
    unsigned int field_5a60;
} st_4480c0_0;

typedef struct st_4480c0_1 {
    char padding_0[30];
    char field_1e;
} st_4480c0_1;

typedef struct st_4480c0_3 {
    char padding_0[125376];
    char field_1e9c0;
} st_4480c0_3;

typedef struct st_4480c0_2 {
    char padding_0[102324];
    int field_18fb4;
} st_4480c0_2;

extern char g_4aebf0;
extern char g_4aedf0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern st_4480c0_2 *g_4b43b8;
extern st_4480c0_3 *g_4b451c;
extern unsigned int g_4b452c;
extern char g_4d48c0;
extern void g_4d48c4;

int sub_4480c0(void)
{
    unsigned int v9;  // ecx
    unsigned int v10;  // edi
    unsigned int v19;  // eax
    st_4480c0_0 *v20;  // ebx
    unsigned int v21;  // 4101
    st_4480c0_0 *iter;  // esi
    unsigned int v23;  // ecx
    unsigned int node;  // ecx
    char v25;  // 4102
    char *v26;  // edx
    char v27;  // 4101
    unsigned int v28;  // cc_dep1
    st_4480c0_0 *v11;  // eax
    unsigned int v29;  // cc_dep2
    char v30;  // 4103
    unsigned int v31;  // eax
    unsigned int v32;  // 4111
    unsigned int v33;  // 4120
    int v34;  // eax
    int v35;  // eax
    st_4480c0_0 *idx;  // esi
    unsigned int v37;  // edx
    unsigned int v38;  // edx
    st_4480c0_0 *idx1;  // edi
    unsigned int v39;  // ecx
    unsigned int index;  // eax
    int v41;  // eax
    int v42;  // eax
    st_4480c0_0 *v43;  // eax
    st_4480c0_1 *iter1;  // esi
    st_4480c0_0 *iter2;  // edx
    char v46;  // 4103
    st_4480c0_0 *v47;  // eax
    char v48;  // 4102
    int v14;  // edx
    unsigned int v15;  // esi
    unsigned int v16;  // esi
    unsigned int v17;  // esi
    int v18;  // eax
    unsigned int v0;  // [bp-0x4c]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v7 = v9;
    v6 = v10;
    idx1 = v11;
    switch (idx1->field_24)
    {
    case 2:
        if (!idx1->field_5988)
        {
            idx = &idx1->field_5990;
            *((unsigned int *)&idx->padding_0[4]) = idx1->field_5990;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0xd);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(13);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                v37 = 1321528399 * *((int *)&idx->padding_0[0]) >> 0x22;
                v4 = (*((int *)&idx->padding_0[0]) == ((v37 >> 31) + v37) * 13 ? 12 : 0xffffffff);
                sub_464970((*((int *)&idx->padding_0[0]) == ((v37 >> 31) + v37) * 13 ? 12 : 0xffffffff));
            }
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                v38 = 1321528399 * *((int *)&idx->padding_0[0]) >> 0x22;
                v3 = (*((int *)&idx->padding_0[0]) - ((v38 >> 31) + v38) * 13 == 12 ? 0xfffffff4 : 1);
                sub_464970((*((int *)&idx->padding_0[0]) - ((v38 >> 31) + v38) * 13 == 12 ? 0xfffffff4 : 1));
            }
            v39 = *((int *)&idx->padding_0[4]);
            if (v39 != *((int *)&idx->padding_0[0]))
                sub_453d90(v39, 10);
        }
        if (!(*((int *)&g_4d48c4) & 524289))
        {
LABEL_4485b2:
            if (!(*((int *)&g_4d48c4) & 258))
            {
                return;
            }
            else if (idx1->field_5988)
            {
                sub_43ef40();
                sub_453d90(v9, 7);
                return;
            }
            else if (idx1->field_5984)
            {
                sub_453d90(v39, 9);
                idx1->field_5984 = idx1->field_5984 - 1;
                (&idx1->field_5978)[idx1->field_5984] = 32;
                return;
            }
            goto LABEL_448672;
        }
        if (!idx1->field_5988)
        {
            index = idx1->field_5990;
            if (index < 88)
            {
                v39 = idx1->field_5984;
                if (v39 < 8)
                {
                    (&idx1->field_5978)[v39] = *((char *)(index + 4853384));
                    goto LABEL_4484bc;
                }
                else
                {
                    idx1->padding_670[21255 + v39] = *((char *)(index + 4853384));
                    break;
                }
            }
            else if (index == 88)
            {
                v41 = idx1->field_5984;
                if (v41 < 8)
                {
                    (&idx1->field_5978)[v41] = 32;
LABEL_4484bc:
                    idx1->field_5984 = idx1->field_5984 + 1;
                    if (idx1->field_5984 >= 8)
                    {
                        sub_40f790();
                        break;
                    }
                }
                else
                {
                    idx1->padding_670[21255 + v41] = 32;
                    break;
                }
            }
            else if (index == 89)
            {
                if (idx1->field_5984)
                {
                    v42 = idx1->field_5984 - 1;
                    idx1->field_5984 = v42;
                    (&idx1->field_5978)[v42] = 32;
                    break;
                }
                else
                {
                    return;
                }
            }
            else
            {
                if (index == 90)
                {
                    v43 = &idx1->field_5978;
                    iter1 = &g_4b451c->padding_0[35816 * g_4b0c90 + 17908 * g_4b0c94 + 280 * g_4b0ca8 + 28 * idx1->field_28 + 30];
                    iter2 = v43;
                    do
                    {
                        v46 = iter2->padding_0[0];
                        iter1->padding_0[0] = iter2->padding_0[0];
                        iter2 = &iter2->padding_0[1];
                        iter1 = &iter1->padding_0[1];
                    } while (v46);
                    v47 = v43;
                    do
                    {
                        v48 = v47->padding_0[0];
                        *(&g_4b451c->padding_0[0] - &v43->padding_0[0] + &v47->padding_0[0] + 125376) = v47->padding_0[0];
                        v47 = &v47->padding_0[1];
                    } while (v48);
                    goto LABEL_44859c;
                }
            }
        }
        else
        {
LABEL_44859c:
            sub_43ef40();
        }
        sub_453d90(v39, 7);
        goto LABEL_4485b2;
    case 3:
        if (idx1->field_2b8 >= 6)
        {
            sub_43efd0();
            sub_43efd0();
            sub_43efd0();
            sub_43efd0();
            sub_43eee0();
        }
LABEL_448672:
        return;
    case 0:
        idx1->field_30 = 30;
        sub_4300d0(0, "bgm/th10_17.wav");
        sub_430150(0, 0x11);
        if (!idx1->field_66c)
        {
            v4 = 0;
            sub_4615a0(g_4b43b8->field_18fb4, &ebx, 20, 0);
            idx1->field_66c = 0;
        }
        v14 = idx1->field_14;
        v2 = 0;
        sub_4615a0(v14, &v4, 116, 0);
        idx1->field_498 = 0;
        sub_43ef40(v14, &v4, 116, 0);
        v1 = 0;
        v15 = g_4b0c90 + 193;
        sub_4615a0(idx1->field_14, &v2, v15, 0);
        *((unsigned int *)&idx1->padding_2bc[12 + 4 * v15]) = 0;
        v0 = 0;
        v16 = g_4b0c94 + 196;
        sub_4615a0(idx1->field_14, &v1, v16, 0);
        *((unsigned int *)&idx1->padding_2bc[12 + 4 * v16]) = 0;
        v17 = g_4b0ca8 + 198;
        sub_4615a0(idx1->field_14, &v0, v17, 0);
        *((unsigned int *)&idx1->padding_2bc[12 + 4 * v17]) = 0;
        if (!sub_461920(idx1->field_454))
        {
            sub_43efa0();
            sub_4619e0(idx1->field_454);
        }
        g_4b0cb0 = 8;
        g_4b0cb4 = 8;
        g_4b452c = &g_4aedf0;
        v18 = sub_431b90(&g_4b451c->padding_0[35816 * g_4b0c90 + 17908 * g_4b0c94 + 8]);
        g_4b452c = &g_4aebf0;
        g_4b0cb0 = 0;
        g_4b0cb4 = 0;
        if (v18 >= 0)
        {
            sub_4067e0(0);
            idx1->field_f8 = 1;
            sub_40f790(0);
            v19 = idx1->field_5998;
            v20 = &idx1->field_5990;
            if (v19)
            {
                v21 = _ccall(14, 15, v19, 0, 0);
                if (v21 & 1)
                    *((unsigned int *)&v20->padding_0[0]) = v19 - 1;
                else
                    *((unsigned int *)&v20->padding_0[0]) = 0;
            }
            else
            {
                *((unsigned int *)&v20->padding_0[0]) = 0;
            }
            iter = &idx1->field_5978;
            v23 = &g_4b451c->field_1e9c0;
            idx1->field_5998 = 91;
            idx1->field_5a60 = 1;
            node = v23;
            do
            {
                v25 = *((char *)node);
                iter->padding_0[node + -1 * v23] = *((char *)node);
                node += 1;
            } while (v25);
            v26 = "        ";
            while (1)
            {
                v27 = iter->padding_0[0];
                v28 = v27;
                v29 = *(v26);
                if (v27 != *(v26))
                    break;
                if (v27)
                {
                    v30 = iter->padding_0[1];
                    v28 = v30;
                    v29 = v26[1];
                    if (v30 != v26[1])
                        break;
                    iter = &iter->padding_0[2];
                    v26 += 2;
                    if (!v30)
                        goto LABEL_448312;
                }
                else
                {
LABEL_448312:
                    v31 = 0;
                    goto LABEL_44831b;
                }
            }
            v32 = _ccall(4, v28, v29, 0);
            v33 = _ccall(12, 0, v32 & 1, v32 & 1);
            v31 = -(v32 & 1) + 1 - (v33 & 1);
LABEL_44831b:
            if (v31)
                sub_464970(-0x1);
            v34 = 8;
            do
            {
            } while (idx1->padding_670[21255 + v34] == 32 && (v34 = (int)(v34 - 1), v34 > 0));
            idx1->field_5984 = v34;
            idx1->field_5988 = 0;
            goto LABEL_44837b;
        }
        else
        {
            v35 = idx1->field_30;
            if (!v35)
            {
                idx1->field_28 = 0xffffffff;
            }
            else if (v35 <= -0x1)
            {
                idx1->field_28 = v35 - 1;
            }
            else
            {
                idx1->field_28 = 0;
            }
            idx1->field_5988 = 1;
            goto LABEL_44837b;
        }
    case 1:
LABEL_44837b:
        if (idx1->field_2b8 <= 6)
            return;
        sub_43ef40();
        return;
    default:
        return;
    }
}
