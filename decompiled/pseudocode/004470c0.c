// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4470c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4470c0_1 {
    struct st_4470c0_0 *field_0;
} st_4470c0_1;

typedef struct st_4470c0_0 {
    char field_0;
} st_4470c0_0;

typedef struct st_4470c0_36 {
    char padding_0[102324];
    int field_18fb4;
} st_4470c0_36;

extern unsigned int g_4a1f94[4];
extern st_4470c0_36 *g_4b43b8;
extern int g_4cee70;
extern char g_4d48c0;
extern void g_4d48c4;
extern char g_4d4d9e;
extern unsigned int g_4d4da0;
extern char g_4d4da1;
extern char g_4d4da2;
extern char g_4d4da3;
extern char g_4d4db0;
extern char g_4d4db1;
extern char g_4d4db2;
extern char g_4d4db3;
extern char g_4d4db4;
extern char g_4d4db5;
extern char g_4d4db6;
extern char g_4d4db7;
extern char g_4d4db8;
extern char g_4d4db9;
extern char g_4d4dbe;
extern char g_4d4dbf;
extern char g_4d4dc0;
extern char g_4d4dc1;
extern char g_4d4dc2;
extern char g_4d4dc3;
extern char g_4d4dc4;
extern char g_4d4dc5;
extern char g_4d4dc6;
extern char g_4d4dcc;
extern char g_4d4dcd;
extern char g_4d4dce;
extern char g_4d4dcf;
extern char g_4d4dd0;
extern char g_4d4dd1;
extern char g_4d4dd2;
extern unsigned int g_4d4ea0;
extern int g_4d4ea4;
extern unsigned int g_4d50d0;
extern char g_4d50d1;
extern char g_4d50d2;
extern char g_4d50d3;
extern unsigned int g_4d51d0;
extern char g_4d51d1;
extern char g_4d51d2;
extern char g_4d51d3;
extern char g_4d5211;
extern char g_4d5212;
extern char g_4d5213;
extern char g_4d5214;
extern char g_4d5215;
extern char g_4d5216;
extern char g_4d5217;
extern char g_4d5218;
extern char g_4d5219;
extern char g_4d521a;
extern char g_4d521b;
extern char g_4d521c;
extern char g_4d521d;
extern char g_4d521e;
extern char g_4d521f;
extern char g_4d5220;
extern char g_4d5221;
extern char g_4d5222;
extern char g_4d5223;
extern char g_4d5224;
extern char g_4d5225;
extern char g_4d5226;
extern char g_4d5227;
extern char g_4d5228;
extern char g_4d5229;
extern char g_4d522a;

unsigned int sub_4470c0(unsigned int a0, unsigned int a1, void* idx)
{
    int v22;  // eax
    unsigned int v30;  // esi
    void* v31;  // esi
    int v32;  // eax
    unsigned int v33;  // ecx
    unsigned int v34;  // edx
    int v35;  // esi
    void* node;  // edi
    void* v37;  // esi
    unsigned int v38;  // edi
    unsigned int v23;  // edx
    unsigned int v39;  // edi
    unsigned int v40;  // ecx
    unsigned int *v41;  // esi
    unsigned int *iter;  // edi
    unsigned int v43;  // eax
    unsigned int v44;  // ecx
    unsigned int *v45;  // esi
    unsigned int *iter1;  // edi
    int iter2;  // eax
    char v48;  // 4149
    unsigned int v24;  // edx
    char v49;  // 4161
    char v50;  // 4173
    unsigned int v51;  // eax
    char v52;  // cl
    int idx1;  // eax
    int v54;  // eax
    unsigned int v55;  // esi
    unsigned int v56;  // esi
    void* v57;  // ebp
    unsigned int v58;  // esi
    int v25;  // eax
    unsigned int v59;  // esi
    unsigned int v60;  // edi
    int v61;  // esi
    int v26;  // ecx
    int v27;  // esi
    unsigned int v28;  // esi
    unsigned int v29;  // esi
    st_4470c0_1 **v0;  // [bp-0xbc]
    st_4470c0_1 **v1;  // [bp-0xac]
    st_4470c0_1 **v2;  // [bp-0x9c]
    st_4470c0_1 **v3;  // [bp-0x8c]
    st_4470c0_1 **v4;  // [bp-0x7c]
    st_4470c0_0 **v5;  // [bp-0x6c]
    st_4470c0_0 **v6;  // [bp-0x5c]
    char *v7;  // [bp-0x4c]
    unsigned int v8;  // [bp-0x44]
    unsigned int v9;  // [bp-0x40]
    char *v10;  // [bp-0x3c], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0x38]
    unsigned int v12;  // [bp-0x34]
    st_4470c0_0 **v13;  // [bp-0x30]
    unsigned int v14;  // [bp-0x2c]
    st_4470c0_0 **v15;  // [bp-0x28]
    unsigned int v16;  // [bp-0x24]
    st_4470c0_1 **v17;  // [bp-0x20]
    unsigned int v18;  // [bp-0x1c]
    st_4470c0_1 **v19;  // [bp-0x18]

    switch ((int)idx[36])
    {
    case 2:
        *((int *)&idx[44]) = (int)idx[40];
        v31 = idx + 0x100;
        *((int *)&v31[4]) = (int)idx[0x100];
        *((int *)&idx[476]) = (int)idx[472];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
        {
            sub_464970(-0x1);
            v19 = (int)idx[1544];
            sub_4619e0((int)idx[1544]);
        }
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            v18 = 1;
            sub_464970(1);
            v17 = (int)idx[1548];
            sub_4619e0((int)idx[1548]);
        }
        if ((int)v31[4] != *((int *)v31))
        {
            sub_453d90();
            sub_43efd0();
            sub_43efa0();
            if ((int)idx[472] > 0)
            {
                v32 = (int)idx[480];
                if (!v32)
                {
                    *((int *)&idx[472]) = 1;
                }
                else if (v32 <= 1)
                {
                    *((unsigned int *)&idx[472]) = v32 - 1;
                }
                else
                {
                    *((int *)&idx[472]) = 1;
                }
                sub_447b20();
            }
            v34 = 1717986919 * (sub_40d840(v33, (int)idx[0x100]) + 9) >> 0x22;
            *((unsigned int *)&idx[480]) = v34 + (v34 >> 31) + 1;
        }
        if (g_4d48c4 & 64 || g_4d48c0 & 64)
        {
            v16 = 0xffffffff;
            sub_464970(-0x1);
            v15 = (int)idx[0x600];
            sub_4619e0((int)idx[0x600]);
        }
        if (g_4d48c4 & 128 || g_4d48c0 & 128)
        {
            v14 = 1;
            sub_464970(1);
            v13 = (int)idx[1540];
            sub_4619e0((int)idx[1540]);
        }
        if ((int)idx[44] != (int)idx[40])
        {
            sub_453d90();
            if ((int)idx[40] / 2 != (int)idx[44] / 2)
            {
                sub_43efd0();
                sub_43efa0();
            }
            sub_43efd0();
            sub_43efa0();
            if ((int)idx[472] > 0)
                sub_447b20();
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            if (!(int)idx[472])
            {
                v35 = 0;
                node = idx + 1648;
                do
                {
                    sub_4615a0(g_4cee70, &v14, v35 + 25, 0);
                } while ((*((unsigned int *)node) = v10, v35 = (int)(v35 + 1), node += 4, v35 < 10));
            }
            v8 = 1;
            sub_464970(1);
            if (!(int)idx[472])
            {
                v37 = idx + 1648;
                v38 = 10;
                do
                {
                    v39 = v38;
                    sub_461970(*((int *)v37), 1);
                } while ((v37 += 4, v38 = v39 - 1, v39 != 1));
            }
            else
            {
                sub_447b20();
            }
            sub_453d90();
        }
        if ((int)idx[0x100] == 4 && (int)idx[40] == 2)
        {
            if (*((int *)&g_4d48c4) & 524547)
            {
                g_4d4ea0 = 0;
                g_4d4ea4 = 0;
            }
            v40 = 64;
            v41 = &g_4d51d0;
            for (iter = &g_4d50d0; v40; v41 += 1)
            {
                v40 -= 1;
                *(iter) = *(v41);
                iter += 1;
            }
            v43 = sub_463910();
            if (v43 == 2)
            {
                sub_477420(&g_4d4da0, 0, 0x100);
                g_4d4dbe = g_4d5211;
                g_4d4dd0 = g_4d5212;
                g_4d4dc0 = g_4d5214;
                g_4d4dce = g_4d5213;
                g_4d4db2 = g_4d5215;
                g_4d4dc2 = g_4d5217;
                g_4d4dc1 = g_4d5216;
                g_4d4dc3 = g_4d5218;
                g_4d4dc4 = g_4d521a;
                g_4d4db7 = g_4d5219;
                g_4d4dc5 = g_4d521b;
                g_4d4dd2 = g_4d521d;
                g_4d4dc6 = g_4d521c;
                g_4d4dd1 = g_4d521e;
                g_4d4db9 = g_4d5220;
                g_4d4db8 = g_4d521f;
                g_4d4db0 = g_4d5221;
                g_4d4dbf = g_4d5223;
                g_4d4db3 = g_4d5222;
                g_4d4db4 = g_4d5224;
                g_4d4dcf = g_4d5226;
                g_4d4db6 = g_4d5225;
                g_4d4db1 = g_4d5227;
                g_4d4db5 = g_4d5229;
                v44 = 64;
                v45 = &g_4d4da0;
                iter1 = &g_4d51d0;
                g_4d4dcd = g_4d5228;
                for (g_4d4dcc = g_4d522a; v44; v45 += 1)
                {
                    v44 -= 1;
                    *(iter1) = *(v45);
                    iter1 += 1;
                }
            }
            else if (!(v43 == 1))
            {
                break;
            }
            iter2 = 0;
            do
            {
                v48 = (&g_4d51d1)[iter2];
                *(iter2 + (char *)&g_4d4da0) = (*(iter2 + (char *)&g_4d50d0) ^ *(iter2 + (char *)&g_4d51d0)) & *(iter2 + (char *)&g_4d51d0);
                v49 = (&g_4d51d2)[iter2];
                (&g_4d4da1)[iter2] = ((&g_4d50d1)[iter2] ^ v48) & v48;
                v50 = (&g_4d51d3)[iter2];
                (&g_4d4da2)[iter2] = ((&g_4d50d2)[iter2] ^ v49) & v49;
                (&g_4d4da3)[iter2] = ((&g_4d50d3)[iter2] ^ v50) & v50;
                iter2 += 4;
            } while (iter2 < 0x100);
            v51 = g_4d4ea0;
            if (v51 >= 12)
            {
                sub_43f1e0();
                sub_453d90();
LABEL_4478cf:
                g_4d4ea0 = 0;
                break;
            }
            else if (*(g_4a1f94[v51] + (char *)&g_4d4da0) & 128)
            {
                g_4d4ea0 = v51 + 1;
                g_4d4ea4 = 0;
                break;
            }
            else
            {
                v52 = 0;
                idx1 = 0;
                do
                {
                    v54 = idx1 + 3;
                    v52 |= *(idx1 + (char *)&g_4d4da0) | (&g_4d4da2)[idx1] | (&g_4d4d9e)[v54];
                    idx1 = v54;
                } while (idx1 < 57);
                if (v52 < 0)
                    goto LABEL_4478cf;
                break;
            }
            g_4d4ea4 = g_4d4ea4 + 1;
            if (g_4d4ea4 > 300)
            {
                g_4d4ea0 = 0;
                g_4d4ea4 = 0;
            }
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            sub_461970(*((int *)((char *)idx + 4 * idx[0x100] + 1504)), v8);
            *((unsigned int *)((char *)idx + 4 * idx[0x100] + 1504)) = 0;
            v55 = (int)idx[40] & 0x80000001;
            if (((int)idx[40] & 0x80000001) < 0)
                v55 = (v55 - 1 | 0xfffffffe) + 1;
            sub_461970(*((int *)((char *)idx + 4 * v55 + 1496)), v9);
            *((unsigned int *)((char *)idx + 4 * v55 + 1496)) = 0;
            v56 = (int)idx[40] / 2;
            sub_461970(*((int *)((char *)idx + 4 * v56 + 1484)), v10);
            *((unsigned int *)((char *)idx + 4 * v56 + 1484)) = 0;
            sub_461970((int)idx[0x600], v11);
            *((st_4470c0_0 ***)&idx[0x600]) = NULL;
            sub_461970((int)idx[1540], v12);
            *((st_4470c0_0 ***)&idx[1540]) = NULL;
            sub_461970((int)idx[1544], v13);
            *((st_4470c0_1 ***)&idx[1544]) = NULL;
            sub_461970((int)idx[1548], v14);
            *((st_4470c0_1 ***)&idx[1548]) = NULL;
            sub_461970((int)idx[1524], v15);
            *((st_4470c0_1 ***)&idx[1524]) = NULL;
            sub_461970((int)idx[1528], v16);
            *((st_4470c0_1 ***)&idx[1528]) = NULL;
            sub_461970((int)idx[1532], v17);
            *((st_4470c0_1 ***)&idx[1532]) = NULL;
            sub_461970((int)idx[1552], v18);
            *((st_4470c0_1 ***)&idx[1552]) = NULL;
            v57 = idx + 1648;
            v58 = 10;
            do
            {
                v59 = v58;
                sub_461970(*((int *)v57), v19);
            } while ((v57 += 4, v58 = v59 - 1, v59 != 1));
            return 1;
        }
        goto LABEL_447b03;
    case 3:
        if ((int)idx[696] >= 6)
        {
            sub_43efd0();
            sub_461970((int)idx[1644], v60);
            *((int *)&idx[1644]) = 0;
            sub_43eee0(v61);
            sub_464940();
        }
LABEL_447b03:
        return 1;
    case 0:
        *((unsigned int *)&idx[48]) = 6;
        *((int *)&idx[40]) = 0;
        *((int *)&idx[264]) = 5;
        v22 = (int)idx[264];
        if (!v22)
        {
            *((unsigned int *)&idx[0x100]) = 1;
        }
        else if (v22 <= 1)
        {
            *((unsigned int *)&idx[0x100]) = v22 - 1;
        }
        else
        {
            *((unsigned int *)&idx[0x100]) = 1;
        }
        v23 = (int)idx[0x100];
        *((unsigned int *)&idx[464]) = 1;
        v24 = 1717986919 * (sub_40d840(a0, v23) + 9) >> 0x22;
        v25 = v24 + (v24 >> 31) + 1;
        *((int *)&idx[480]) = v25;
        if (!v25)
        {
            *((int *)&idx[472]) = 0;
        }
        else if (v25 <= 0)
        {
            *((unsigned int *)&idx[472]) = v25 - 1;
        }
        else
        {
            *((int *)&idx[472]) = 0;
        }
        *((unsigned int *)&idx[680]) = 1;
        if (!(int)idx[1644])
        {
            v18 = &idx;
            sub_4615a0(g_4b43b8->field_18fb4, &idx, 20, 0);
            *((int *)&idx[1644]) = v61;
        }
        v26 = (int)idx[20];
        v14 = &v61;
        sub_4615a0(v26, &v61, 114, 0);
        *((unsigned int *)&idx[1168]) = v18;
        sub_43ef40(v26, &v61, 114, 0);
        v27 = (int)idx[40] / 2 + 193;
        v10 = &v18;
        sub_4615a0((int)idx[20], &v18, v27, 0);
        *((int **)((char *)idx + 4 * v27 + 712)) = &v61;
        v28 = (int)idx[40] & 0x80000001;
        if (((int)idx[40] & 0x80000001) < 0)
            v28 = (v28 - 1 | 0xfffffffe) + 1;
        v29 = v28 + 196;
        v7 = &v14;
        sub_4615a0((int)idx[20], &v14, v29, 0);
        *((unsigned int **)((char *)idx + 4 * v29 + 712)) = &v18;
        v30 = (int)idx[0x100] + 198;
        v6 = &v10;
        sub_4615a0((int)idx[20], &v10, v30, 0);
        *((unsigned int **)((char *)idx + 4 * v30 + 712)) = &v14;
        v5 = &v7;
        sub_4615a0((int)idx[20], &v7, 206, 0);
        *((char ***)&idx[0x600]) = &v10;
        v4 = &v6;
        sub_4615a0((int)idx[20], &v6, 207, 0);
        *((char ***)&idx[1540]) = &v7;
        v3 = &v5;
        sub_4615a0((int)idx[20], &v5, 208, 0);
        *((st_4470c0_0 ****)&idx[1544]) = &v6;
        v2 = &v4;
        sub_4615a0((int)idx[20], &v4, 209, 0);
        *((st_4470c0_0 ****)&idx[1548]) = &v5;
        v1 = &v3;
        sub_4615a0((int)idx[20], &v3, 203, 0);
        *((st_4470c0_1 ****)&idx[1524]) = &v4;
        v0 = &v2;
        sub_4615a0((int)idx[20], &v2, 0xcc, 0);
        *((st_4470c0_1 ****)&idx[1528]) = &v3;
        sub_4615a0((int)idx[20], &v1, 205, 0);
        *((st_4470c0_1 ****)&idx[1532]) = &v2;
        sub_4615a0((int)idx[20], &v0, 210, 0);
        *((st_4470c0_1 ****)&idx[1552]) = &v1;
    case 1:
        if ((int)idx[696] <= 6)
            return 1;
        sub_43ef40();
        return 1;
    default:
        return 1;
    }
}
