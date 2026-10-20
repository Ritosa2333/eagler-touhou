// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x481a74; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

typedef struct st_481a74_1 {
    st_47e26b_0 field_0;
    unsigned int field_4;
} st_481a74_1;

typedef struct st_481a74_0 {
    char field_0;
} st_481a74_0;

extern st_481a74_0 *g_4b4318;
extern unsigned int g_4b4328;

unsigned int * sub_481a74(unsigned int *a0, st_481a74_1 *a1, int a2, st_481a74_1 *a3, int a4)
{
    unsigned int v21;  // ebx
    unsigned int v22;  // esi
    char *v30;  // eax
    unsigned int v32;  // edx
    unsigned int *v33;  // eax
    unsigned int *v35;  // eax
    unsigned int v36;  // eax
    unsigned int *v37;  // eax
    unsigned int v23;  // edi
    unsigned int *v38;  // eax
    unsigned int v39;  // edx
    unsigned int *v40;  // eax
    unsigned int *v41;  // eax
    unsigned int *v42;  // eax
    char v44;  // al
    char v45;  // al
    unsigned int *v46;  // eax
    unsigned int *v24;  // eax
    unsigned int *v48;  // eax
    unsigned int *v49;  // eax
    st_481a74_1 *v50;  // eax
    unsigned int v51;  // edx
    st_481a74_1 *v52;  // ecx
    int v53;  // eax
    st_481a74_1 *v54;  // eax
    st_481a74_1 *v55;  // ecx
    unsigned int v25;  // ecx
    char *v26;  // ecx
    unsigned int v27;  // ebx
    unsigned int v28;  // eax
    unsigned int v29;  // eax
    char *v0;  // [bp-0x9c], Other Possible Types: int
    char *v1;  // [bp-0x74]
    unsigned int v2;  // [bp-0x68]
    unsigned int v3;  // [bp-0x64]
    unsigned int v4;  // [bp-0x60]
    unsigned int v5;  // [bp-0x5c]
    char v6;  // [bp-0x58]
    char v7;  // [bp-0x50]
    unsigned int v8[2];  // [bp-0x48]
    char v9;  // [bp-0x40]
    char v10;  // [bp-0x38]
    char v11;  // [bp-0x30]
    unsigned int v12;  // [bp-0x28]
    unsigned int v13;  // [bp-0x24]
    char v14;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v15;  // [bp-0x1c]
    unsigned int v16;  // [bp-0x18]
    unsigned int v17;  // [bp-0x14]
    char v18;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v19;  // [bp-0xc]
    char v20;  // [bp-0x5]

    v5 = v21;
    v4 = v22;
    v13 &= 0xffff0000;
    v3 = v23;
    v12 = 0;
    v20 = 0;
    if (!g_4b4318->field_0)
    {
        if (!a4)
        {
            v54 = a1;
            if (v54->field_0.field_0)
            {
                if (*((int *)&v54->field_0.field_4) & 0x100 || (v55 = a3, !v55->field_0.field_0))
                    goto LABEL_481f12;
                sub_47ec26(&v7, 1, v55, &v6, 32, a0, v54);
                sub_47ec6e(&v6, 32, a0, v54);
                sub_47e9ae();
            }
            else
            {
                v54 = a3;
                if (!v54->field_0.field_0)
                    goto LABEL_481f29;
LABEL_481f12:
                sub_47ec26(a0, 1, v54);
            }
        }
        else
        {
LABEL_481f29:
            v2 = 1;
LABEL_481f2b:
            sub_47e1ce(v0);
        }
        return a0;
    }
    if (g_4b4318->field_0 == 36)
    {
        sub_47f037(&v14, &a2, &v20, a4);
        if (v14)
        {
            v24 = a0;
            *(v24) = v14;
            v24[1] = v15;
            return v24;
        }
    }
    v26 = &g_4b4318->field_0;
    v17 &= 0xffff0000;
    v2 = 32;
    v16 = 0;
    v27 = g_4b4318->field_0 - (((unsigned int)((g_4b4318->field_0 < 65) + 1) & 43) + 22);
    v15 &= 0xffff0000;
    v14 = 0;
    while (1)
    {
        v28 = v27;
        v29 = v28 - 4;
        if (v28 != 4)
        {
            if (v29 != 1)
            {
                if (v29 != 4)
                    break;
                if ((char)~(g_4b4328 >> 1) & 1)
                {
                    v36 = sub_47dc0b(8);
                    v36 = v36;
                    if (v16)
                    {
                        v1 = &v11;
                        v30 = &v10;
                        goto LABEL_481b42;
                    }
                }
            }
            else if ((char)~(g_4b4328 >> 1) & 1)
            {
                v36 = sub_47dc0b(9);
                v36 = v36;
                if (!v14)
                    goto LABEL_481bdc;
                v33 = sub_47ec92(sub_47ec6e(v8, 32, &v9), v32, v8, 32);
                v14 = *(v33);
                v15 = v33[1];
            }
        }
        else
        {
            if ((char)~(g_4b4328 >> 1) & 1 && (char)~(g_4b4328 >> 0x11) & 1)
            {
                v36 = sub_47dc0b(7);
                v36 = v36;
                if (v16)
                {
                    v1 = &v7;
                    v30 = &v6;
LABEL_481b42:
                    v35 = sub_47ec92(sub_47ec6e(v30, 32), v32, v30, 32);
                    v16 = *(v35);
                    v17 = v35[1];
                }
                else
                {
LABEL_481bdc:
                    sub_47e896(v36);
                }
            }
        }
        g_4b4318 = g_4b4318 + 1;
        if (g_4b4318->field_0 == 36)
        {
            sub_47f037(&v18, &a2, &v20, a4);
            if (v18)
            {
                v24 = a0;
                *(v24) = v18;
                v24[1] = v19;
                return v24;
            }
        }
        v26 = &g_4b4318->field_0;
        v27 = g_4b4318->field_0 - (((unsigned int)((g_4b4318->field_0 < 65) + 1) & 43) + 22);
    }
    if (*(v26))
        g_4b4318 = v26 + 1;
    if (v27 <= 31)
    {
        sub_47e56d(a2);
        v37 = sub_47e9ae(&v6, &v18);
        v18 = *(v37);
        v19 = v37[1];
        if (v16)
        {
            sub_47ec6e(&v7, 32, &v6, &v16);
            v38 = sub_47e9ae(&v7, 32, &v6, &v16);
            v18 = *(v38);
            v19 = v38[1];
        }
        if (v14)
        {
            sub_47ec6e(&v7, 32, &v6, &v18);
            v40 = sub_47e9ae(&v7, 32, &v6, &v18);
            v18 = *(v40);
            v19 = v40[1];
        }
        if (!((char)v27 & 16))
        {
LABEL_481d8b:
            v45 = v27;
            if ((char)~(g_4b4328 >> 1) & 1)
            {
                if ((v45 & 12) != 12)
                    goto LABEL_481dea;
                if (!a4)
                {
                    sub_480665(&v7, &v6, &v18);
                    v46 = sub_47e9ae(&v6, &v18);
                    v18 = *(v46);
                    v19 = v46[1];
                    goto LABEL_481dea;
                }
            }
            else
            {
                if ((v45 & 12) == 12)
                    sub_47ddc1(&v18, v32, sub_480665(&v6));
LABEL_481dea:
                if ((char)v27 & 2)
                {
                    v48 = sub_47ec4a(&v6, "volatile ", &v18);
                    v18 = *(v48);
                    v19 = v48[1];
                }
                if ((char)v27 & 1)
                {
                    v49 = sub_47ec4a(&v6, "const ", &v18);
                    v18 = *(v49);
                    v19 = v49[1];
                }
                if (!a4)
                {
                    v50 = a1;
                    if (v50->field_0.field_0)
                    {
                        v51 = *((int *)&v50->field_0.field_4);
                        if (!(v51 & 0x100) && (v52 = a3, v52->field_0.field_0))
                        {
                            sub_47ec02(v8, 32, v52, &v7, 32, &v6, v50);
                            sub_47ec6e(&v7, 32, &v6, v50);
                            v53 = sub_47e9ae();
                            goto LABEL_481ea6;
                        }
                        else
                        {
                            if (!(v51 & 0x800))
                                goto LABEL_481e98;
                            v18 = v50->field_0.field_0;
                            v19 = v51;
                        }
                    }
                    else
                    {
                        v50 = a3;
                        if (v50->field_0.field_0)
                        {
LABEL_481e98:
                            v53 = sub_47ec02(&v6, 32, v50);
LABEL_481ea6:
                            sub_47e7bf(v53);
                        }
                    }
                }
                v25 = v19 | 0x100;
                if (v20)
                    v25 |= 0x2000;
                v24 = a0;
                *(v24) = v18;
                v24[1] = v25;
                return v24;
            }
        }
        else if (!a4)
        {
            if ((char)a2)
            {
                v41 = sub_47ec4a(&v6, "::", &v18);
                v19 = v41[1];
                v0 = &v18;
                v18 = *(v41);
                if (g_4b4318->field_0)
                {
                    sub_4814b0(&v7, &v6);
                    v42 = sub_47e9ae(&v6);
                }
                else
                {
                    v42 = sub_47ec26(&v6, 1);
                }
                v18 = *(v42);
                v19 = v42[1];
            }
            else
            {
                if (!g_4b4318->field_0)
                    goto LABEL_481d71;
                sub_47ddc1(&v18, v32, sub_4814b0(&v6));
            }
            v44 = g_4b4318->field_0;
            if (!g_4b4318->field_0)
            {
LABEL_481d71:
                sub_47e4af(&v18, v39, 1);
                goto LABEL_481d8b;
            }
            else
            {
                g_4b4318 = g_4b4318 + 1;
                if (v44 == 64)
                    goto LABEL_481d8b;
            }
        }
    }
    v0 = 2;
    goto LABEL_481f2b;
}
