// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4489e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4489e0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    char padding_34[196];
    unsigned int field_f8;
    char padding_fc[444];
    int field_2b8;
    char padding_2bc[408];
    int field_454;
    char padding_458[68];
    int field_49c;
    char padding_4a0[460];
    int field_66c;
    char padding_670[21256];
    char field_5978;
    char field_5979;
    char padding_597a[10];
    int field_5984;
    char padding_5988[8];
    unsigned int field_5990;
    unsigned int field_5994;
    unsigned int field_5998;
    char padding_599c[196];
    unsigned int field_5a60;
    char padding_5a64[20];
    unsigned int field_5a78;
    char padding_5a7c[4];
    unsigned int field_5a80;
} st_4489e0_0;

typedef struct st_4489e0_2 {
    char padding_0[28];
    struct st_4489e0_1 *field_1c;
} st_4489e0_2;

typedef struct st_4489e0_1 {
    char padding_0[104];
    unsigned int field_68;
} st_4489e0_1;

typedef struct st_4489e0_3 {
    char padding_0[125376];
    char field_1e9c0;
} st_4489e0_3;

extern unsigned int g_4ad138;
extern char g_4aedf0;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern st_4489e0_2 *g_4b4518;
extern st_4489e0_3 *g_4b451c;
extern unsigned int g_4b452c;
extern char g_4d48c0;
extern void g_4d48c4;

void sub_4489e0(st_4489e0_0 *idx)
{
    unsigned int v7;  // edi
    unsigned int v17;  // eax
    unsigned int v18;  // 4101
    st_4489e0_0 *iter;  // esi
    unsigned int v20;  // eax
    unsigned int node;  // eax
    char v22;  // 4102
    char *v23;  // ecx
    char v24;  // 4101
    unsigned int v25;  // cc_dep1
    unsigned int v26;  // cc_dep2
    unsigned int v9;  // eax
    char v27;  // 4103
    unsigned int v28;  // eax
    unsigned int v29;  // 4111
    unsigned int v30;  // 4120
    int v31;  // eax
    st_4489e0_0 *v32;  // esi
    unsigned int v33;  // edx
    unsigned int v34;  // edx
    unsigned int index;  // eax
    int v36;  // ecx
    unsigned int v10;  // 4101
    int v37;  // eax
    int v38;  // eax
    st_4489e0_0 *v39;  // esi
    unsigned int v40;  // eax
    unsigned int v41;  // edx
    st_4489e0_0 *iter1;  // eax
    char v43;  // 4102
    unsigned int v44;  // esi
    int v45;  // ebp
    st_4489e0_0 *v46;  // ebp
    int v11;  // esi
    int v47;  // ebx
    st_4489e0_0 *iter2;  // esi
    int v49;  // ebx
    unsigned int v50;  // edi
    st_4489e0_0 *v12;  // ebx
    int v13;  // eax
    int v14;  // eax
    st_4489e0_0 *v15;  // esi
    st_4489e0_0 *v16;  // ebp
    char *v0;  // [bp-0x68], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x64]
    unsigned int v3;  // [bp-0x4c]
    char v4;  // [bp-0x48]
    unsigned int v5;  // [bp-0x4]

    v5 = g_4ad138 ^ &v4;
    v3 = v7;
    switch (idx->field_24)
    {
    case 2:
        v15 = &idx->field_28;
        *((unsigned int *)&v15->padding_0[4]) = idx->field_28;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (*((int *)&v15->padding_0[4]) != *((int *)&v15->padding_0[0]))
            sub_453d90();
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            goto LABEL_448f7f;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            v16 = &idx->field_5990;
            idx->field_5a78 = *((int *)&v15->padding_0[0]);
            v17 = *((int *)&v16->padding_0[8]);
            if (v17)
            {
                v18 = _ccall(14, 15, v17, 0, 0);
                if (v18 & 1)
                    *((unsigned int *)&v16->padding_0[0]) = v17 - 1;
                else
                    *((unsigned int *)&v16->padding_0[0]) = 0;
            }
            else
            {
                *((unsigned int *)&v16->padding_0[0]) = 0;
            }
            idx->field_5998 = 91;
            idx->field_5a60 = 1;
            sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
            g_4b4518->field_1c->field_68 = 8;
            iter = &idx->field_5978;
            v20 = &g_4b451c->field_1e9c0;
            node = v20;
            do
            {
                v22 = *((char *)node);
                iter->padding_0[node + -1 * v20] = *((char *)node);
                node += 1;
            } while (v22);
            idx->field_5984 = 0;
            v23 = "        ";
            while (1)
            {
                v24 = iter->padding_0[0];
                v25 = v24;
                v26 = *(v23);
                if (v24 != *(v23))
                    break;
                if (v24)
                {
                    v27 = iter->padding_0[1];
                    v25 = v27;
                    v26 = v23[1];
                    if (v27 != v23[1])
                        break;
                    iter = &iter->padding_0[2];
                    v23 += 2;
                    if (!v27)
                        goto LABEL_448c4c;
                }
                else
                {
LABEL_448c4c:
                    v28 = 0;
                    goto LABEL_448c55;
                }
            }
            v29 = _ccall(4, v25, v26, 0);
            v30 = _ccall(12, 0, v29 & 1, v29 & 1);
            v28 = -(v29 & 1) + 1 - (v30 & 1);
LABEL_448c55:
            if (v28)
                sub_464970(-0x1);
            v31 = 8;
            do
            {
            } while (idx->padding_670[21255 + v31] == 32 && (v31 = (int)(v31 - 1), v31 > 0));
            idx->field_5984 = v31;
            sub_453d90();
            sub_43ef40();
            goto LABEL_448f7f;
        }
    case 3:
        v32 = &idx->field_5990;
        *((unsigned int *)&v32->padding_0[4]) = idx->field_5990;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0xd);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(13);
        if (g_4d48c4 & 64 || g_4d48c0 & 64)
        {
            v33 = 1321528399 * *((int *)&v32->padding_0[0]) >> 0x22;
            v1 = (*((int *)&v32->padding_0[0]) == ((v33 >> 31) + v33) * 13 ? 12 : 0xffffffff);
            sub_464970((*((int *)&v32->padding_0[0]) == ((v33 >> 31) + v33) * 13 ? 12 : 0xffffffff));
        }
        if (g_4d48c4 & 128 || g_4d48c0 & 128)
        {
            v34 = 1321528399 * *((int *)&v32->padding_0[0]) >> 0x22;
            v0 = (*((int *)&v32->padding_0[0]) - ((v34 >> 31) + v34) * 13 == 12 ? 0xfffffff4 : 1);
            sub_464970((*((int *)&v32->padding_0[0]) - ((v34 >> 31) + v34) * 13 == 12 ? 0xfffffff4 : 1));
        }
        if (*((int *)&v32->padding_0[4]) != *((int *)&v32->padding_0[0]))
            sub_453d90();
        if (*((int *)&g_4d48c4) & 524289)
        {
            index = *((int *)&v32->padding_0[0]);
            if (index < 88)
            {
                v36 = idx->field_5984;
                if (v36 < 8)
                {
                    (&idx->field_5978)[v36] = *((char *)(index + 4853384));
                    goto LABEL_448d90;
                }
                else
                {
                    idx->padding_670[21255 + v36] = *((char *)(index + 4853384));
                    goto LABEL_448e8e;
                }
            }
            if (index == 88)
            {
                v37 = idx->field_5984;
                if (v37 < 8)
                {
                    (&idx->field_5978)[v37] = 32;
LABEL_448d90:
                    idx->field_5984 = idx->field_5984 + 1;
                    if (idx->field_5984 >= 8)
                    {
                        sub_40f790();
                        goto LABEL_448e8e;
                    }
                }
                else
                {
                    idx->padding_670[21255 + v37] = 32;
                    goto LABEL_448e8e;
                }
            }
            if (index != 89)
            {
                if (index == 90)
                {
                    sub_453d90();
                    sub_46cbbb(&v45, "th12_%.2d.rpy", idx->field_28 + 1);
                    sub_43b7c0();
                    v39 = &idx->field_5978;
                    sub_43bc10(&v45, v39, 0);
                    v40 = sub_43b6f0(&v45);
                    v41 = &g_4b451c->field_1e9c0;
                    (&idx->field_5a80)[idx->field_28] = v40;
                    iter1 = v39;
                    do
                    {
                        v43 = iter1->padding_0[0];
                        *((char *)(iter1 + v41 - (char *)v39)) = iter1->padding_0[0];
                        iter1 = &iter1->padding_0[1];
                    } while (v43);
                    sub_43ef40();
                }
LABEL_448e8e:
                sub_453d90();
                goto LABEL_448e98;
            }
            if (!idx->field_5984)
                goto LABEL_448f7f;
            v38 = idx->field_5984 - 1;
            idx->field_5984 = v38;
            (&idx->field_5978)[v38] = 32;
            goto LABEL_448e8e;
        }
LABEL_448e98:
        if (!(*((int *)&g_4d48c4) & 258))
            goto LABEL_448f7f;
        if (!idx->field_5984)
        {
LABEL_448b07:
            sub_43ef40();
            goto LABEL_448f7f;
        }
        else
        {
            sub_453d90();
            idx->field_5984 = idx->field_5984 - 1;
            (&idx->field_5978)[idx->field_5984] = 32;
            goto LABEL_448f7f;
        }
    case 4:
        if (idx->field_2b8 >= 6)
        {
            sub_43efd0();
            sub_461970(idx->field_66c, v44);
            idx->field_66c = 0;
            sub_43eee0(v45);
            sub_464940();
            if (g_4b4518)
            {
                sub_43b450(g_4b4518);
                sub_46ca4f(g_4b4518);
            }
            sub_4300d0(0, "bgm/th12_01.wav");
            v1 = 0;
            sub_430150(0);
            v46 = &idx->field_5a80;
            v47 = 25;
            iter2 = v46;
            do
            {
                v49 = v47;
                v50 = *((int *)&iter2->padding_0[0]);
                if (v50)
                {
                    sub_43b450(v50);
                    sub_46ca4f(v50);
                }
            } while ((iter2 += 4, v47 = (int)(v49 - 1), v49 != 1));
            sub_477420(v46, v47, 400);
        }
LABEL_448f7f:
        break;
    case 0:
        idx->field_30 = 25;
        idx->field_f8 = 1;
        v9 = idx->field_30;
        if (v9)
        {
            v10 = _ccall(14, 15, v9, 0, 0);
            if (v10 & 1)
                idx->field_28 = v9 - 1;
            else
                idx->field_28 = 0;
        }
        else
        {
            idx->field_28 = 0;
        }
        g_4b452c = &g_4aedf0;
        g_4b0cb0 = 8;
        g_4b0cb4 = 8;
        v11 = 1;
        v12 = &idx->field_5a80;
        do
        {
            v0 = &v4;
            sub_46cbbb(&v4, "th12_%.2d.rpy", v11);
            *((unsigned int *)&v12->padding_0[0]) = sub_43b6f0(&v4);
            v11 += 1;
            v12 = &v12->padding_0[4];
        } while (v11 <= 25);
        if (!sub_461920(idx->field_454))
        {
            v0 = NULL;
            sub_4615a0(idx->field_14, &v45, 99, 0);
            v13 = idx->field_454;
            idx->field_454 = v13;
            sub_4619e0(v13);
        }
        v14 = idx->field_14;
        sub_4615a0(v14, &v0, 117, 0);
        idx->field_49c = v13;
        sub_43ef40(v14, &v0, 117, 0);
    case 1:
        if (idx->field_2b8 > 6)
            goto LABEL_448b07;
        goto LABEL_448f7f;
    }
    _security_check_cookie();
    return;
}
