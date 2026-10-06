// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4844af; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CPINFO {
    unsigned int MaxCharSize;
    Byte DefaultChar[2];
    Byte LeadByte[12];
} CPINFO;

typedef struct st_4844af_0 {
    char padding_0[254];
    unsigned short field_fe;
} st_4844af_0;

typedef struct st_4844af_4 {
    char padding_0[256];
    unsigned short field_100;
} st_4844af_4;

typedef struct st_4844af_3 {
    char field_0;
    char padding_1[5];
    char field_6;
    char field_7;
} st_4844af_3;

extern char g_49e758;
extern char g_49e8d8;

int sub_4844af(void* idx)
{
    void* v14;  // eax
    int i;  // eax
    st_4844af_4 *v24;  // ecx
    unsigned int v25;  // edi
    unsigned int v26;  // eax
    void* v27;  // edi
    unsigned int v28;  // eax
    char *v16;  // eax
    unsigned int j;  // edi
    unsigned int v18;  // ecx
    st_4844af_0 *v19;  // eax
    st_4844af_0 *v20;  // ecx
    unsigned int v21;  // ecx
    st_4844af_3 *v22;  // ecx
    unsigned int v23;  // ecx
    void* v0;  // [bp-0x50]
    unsigned int v1;  // [bp-0x4c]
    unsigned int v2;  // [bp-0x48]
    st_4844af_0 *v3;  // [bp-0x44]
    st_4844af_0 *v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x3c]
    st_4844af_4 *v6;  // [bp-0x38]
    int v7;  // [bp-0x34]
    unsigned int v8;  // [bp-0x30]
    unsigned int v9;  // [bp-0x2c]
    unsigned int v10;  // [bp-0x28]
    unsigned int iter;  // [bp-0x24]
    st_4844af_0 *v12;  // [bp-0x20], Other Possible Types: st_4844af_3 *
    CPINFO v13;  // [bp-0x1c]

    v8 = 0;
    v12 = NULL;
    iter = 0;
    v9 = 0;
    v10 = 0;
    v0 = idx;
    v1 = 0;
    if ((int)idx[20])
    {
        v14 = idx + 4;
        if (*((int *)v14) || !sub_47a12b(&v0, 0, (short)idx[48], 4100, v14))
        {
            v8 = sub_4777ce(4);
            v12 = sub_477813(384, 2);
            iter = sub_477813(384, 1);
            v9 = sub_477813(384, 1);
            v10 = sub_477813(0x101, 1);
            if (v8 && v12 && v10 && iter && v9)
            {
                *((unsigned int *)v8) = 0;
                i = 0;
                do
                {
                    *((char *)(i + v10)) = i;
                    i += 1;
                } while (i < 0x100);
                if (GetCPInfo((int)idx[4], &v13) && v13.MaxCharSize <= 5)
                {
                    v7 = *((unsigned short *)&v13);
                    if (v7 > 1 && *(&v13.LeadByte[0]))
                    {
                        v16 = &v13.LeadByte[1];
                        do
                        {
                            if (!*(v16))
                                break;
                            j = *(v16 - 1);
                            for (v18 = *(v16); j <= v18; j += 1)
                            {
                                *((char *)(j + v10)) = 32;
                                v18 = *(v16);
                            }
                            v16 += 2;
                        } while (*(v16 - 1));
                    }
                    v3 = v12 + 1;
                    if (sub_48384c(0, 1, v10, 0x100, v3, (int)idx[4], 0, 0) && sub_48364d(0, (int)idx[20], 0x100, v10 + 1, 0xff, iter + 129, 0xff, (int)idx[4], 0) && sub_48364d(0, (int)idx[20], 0x200, v10 + 1, 0xff, v9 + 129, 0xff, (int)idx[4], 0))
                    {
                        v19 = v12;
                        v20 = &v19->field_fe;
                        *((unsigned short *)&v20->padding_0[0]) = 0;
                        v4 = v20;
                        v21 = iter + 128;
                        *((char *)(iter + 127)) = 0;
                        *((char *)(v9 + 127)) = 0;
                        *((char *)v21) = 0;
                        v2 = v21;
                        v5 = v9 + 128;
                        *((char *)v5) = 0;
                        if (v7 > 1 && *(&v13.LeadByte[0]))
                        {
                            v22 = &v13.LeadByte[1];
                            v12 = &v13.LeadByte[1];
                            do
                            {
                                if (!v22->field_0)
                                    break;
                                v23 = *(&v22->field_0 - 1);
                                iter = v23;
                                if (v23 <= v22->field_0)
                                {
                                    v24 = &v19->padding_0[2 * iter + 0x100];
                                    while (1)
                                    {
                                        iter += 1;
                                        *((unsigned short *)&v24->padding_0[0]) = 0x8000;
                                        v6 = &v24->padding_0[2];
                                        if (iter > v12->field_0)
                                            break;
                                        v24 = v6;
                                    }
                                }
                                v22 = &v12->padding_1[1];
                                v12 = v22;
                            } while (*(&v22->field_0 - 1));
                        }
                        sub_47a330(v19, v19 + 2, 254);
                        sub_47a330(iter, iter + 0x100, 127);
                        sub_47a330(v9, v9 + 0x100, 127);
                        if ((int)idx[192] && !InterlockedDecrement((int)idx[192]))
                        {
                            sub_46c941((int)idx[196] - 254, v25);
                            sub_46c941((int)idx[0xcc] - 128, (int)idx[196] - 254);
                            sub_46c941((int)idx[208] - 128, (int)idx[0xcc] - 128);
                            sub_46c941((int)idx[192], (int)idx[208] - 128);
                        }
                        *((unsigned int *)v8) = 1;
                        *((unsigned int *)&idx[192]) = v8;
                        *((st_4844af_0 **)&idx[200]) = v3;
                        *((st_4844af_0 **)&idx[196]) = v4;
                        *((unsigned int *)&idx[0xcc]) = v2;
                        *((unsigned int *)&idx[208]) = v5;
                        *((int *)&idx[172]) = v7;
                        sub_46c941(v10, v25);
                        return v26;
                    }
                }
            }
        }
        sub_46c941(v8, v25);
        sub_46c941(v12, v8);
        sub_46c941(iter, v12);
        sub_46c941(v9, iter);
        sub_46c941(v10, v25);
        return v26;
    }
    else
    {
        v27 = idx + 192;
        if (*((int *)v27))
            InterlockedDecrement(*((int *)v27));
        *((unsigned int *)v27) = 0;
        *((st_4844af_0 **)&idx[196]) = NULL;
        *((wchar **)&idx[200]) = L"         (((((                  H\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x84\x84\x84\x84\x84\x84\x84\x84\x84\x84\x10\x10\x10\x10\x10\x10\x10\x81\x81\x81\x81\x81\x81\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x10\x10\x10\x10\x10\x10\x82\x82\x82\x82\x82\x82\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x10\x10\x10\x10 ";
        *((char **)&idx[0xcc]) = &g_49e758;
        *((char **)&idx[208]) = &g_49e8d8;
        *((int *)&idx[172]) = 1;
        return v28;
    }
}
