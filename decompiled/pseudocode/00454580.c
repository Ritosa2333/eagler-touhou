// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454580; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454580_5 {
    struct st_454580_6 *field_0;
    char padding_4[4];
    struct st_454580_4 *field_8;
    int field_c;
} st_454580_5;

typedef struct st_454580_11 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned short field_10;
} st_454580_11;

typedef struct st_454580_6 {
    struct st_454580_0 *field_0;
} st_454580_6;

typedef struct st_454580_1 {
    unsigned int field_0;
} st_454580_1;

typedef struct st_454580_0 {
    char padding_0[8];
    struct st_454580_1 *field_8;
    char padding_c[32];
    struct st_454580_1 *field_2c;
    char padding_30[28];
    struct st_454580_1 *field_4c;
} st_454580_0;

typedef struct st_454580_4 {
    char padding_0[4];
    unsigned int field_4;
} st_454580_4;

extern int g_4a313c;
extern int g_4a3160;
extern st_454580_6 *g_4cf4e8;
extern unsigned int g_4cf4f8;
extern unsigned int g_4d0e70[4];
extern unsigned int g_4d0e78;
extern unsigned int g_4d1410[4];
extern char g_4d4770;

int sub_454580(int a0)
{
    unsigned int v20;  // eax
    st_454580_5 *index;  // esi
    int v30;  // edi
    char *v31;  // ecx
    unsigned int v32;  // eax
    unsigned int v33;  // eax
    st_454580_0 **v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // edi
    unsigned int *v25;  // ecx
    unsigned int v26;  // eax
    unsigned int v27;  // ebx
    st_454580_11 *idx;  // eax
    unsigned int v29;  // ebp
    st_454580_0 **v0;  // [bp-0x70], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x6c]
    int v2;  // [bp-0x68], Other Possible Types: unsigned int
    st_454580_5 *v3;  // [bp-0x64], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x60]
    int v5;  // [bp-0x50]
    char v6;  // [bp-0x48]
    unsigned int v7;  // [bp-0x44]
    unsigned int v8;  // [bp-0x40]
    unsigned int v9;  // [bp-0x3c]
    unsigned int v10;  // [bp-0x38]
    unsigned short v11;  // [bp-0x34], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x30]
    int v13;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v14;  // [bp-0x28]
    char *v15;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int v16;  // [bp-0x20]
    unsigned int v17;  // [bp-0x1c]
    unsigned int v18;  // [bp-0x18]
    unsigned int v19;  // [bp-0x14]

    v5 = a0;
    if (!g_4cf4f8)
        return v20;
    v22 = index->field_0;
    if (v22)
    {
        *(v22)->field_8(v22);
        index->field_0 = NULL;
    }
    v23 = 0;
    v4 = v24;
    if (index->field_c > 0)
    {
        v25 = &g_4d0e78;
        do
        {
            if (*((int *)(*(v25) + 4)) == index->field_8->field_4)
            {
                v3 = index;
                (*((int *)&g_4cf4e8->field_0->padding_c[8]))(g_4cf4e8, g_4d0e70[6 * v23], index);
                _security_check_cookie(v19 ^ &v3);
                return v26;
            }
        } while ((v23 += 1, v25 += 24, v23 < index->field_c));
    }
    v3 = v27;
    if (!g_4d1410[index->field_8->field_4])
    {
        do
        {
            Sleep(10);
            if (*((int *)&g_4d4770) == 2)
            {
                _security_check_cookie();
                return v32;
            }
        } while (!g_4d1410[index->field_8->field_4]);
    }
    if (sub_46dc03(g_4d1410[index->field_8->field_4], "RIFF", 4))
    {
        sub_464220(&g_4a313c, a0);
        goto LABEL_4547b9;
    }
    if (sub_46dc03(g_4d1410[index->field_8->field_4] + 8, "WAVE", 4))
    {
        v2 = a0;
    }
    else
    {
        v2 = "fmt ";
        idx = sub_453620("fmt ");
        if (!idx)
        {
            v1 = v29;
        }
        else
        {
            v7 = idx->field_0;
            v8 = idx->field_4;
            v9 = idx->field_8;
            v10 = idx->field_c;
            v11 = idx->field_10;
            v30 = sub_453620("data");
            if (!v30)
            {
                v0 = v22;
            }
            else
            {
                v15 = 0;
                v0 = 0;
                v11 = 0;
                v12 = 0;
                v13 = 0;
                v14 = 0;
                v16 = 0;
                v17 = 0;
                v18 = 0;
                v19 = 0;
                v15 = &v6;
                v11 = 36;
                v12 = 32968;
                v13 = v5;
                if ((*((int *)&g_4cf4e8->field_0->padding_c[0]))(g_4cf4e8, &v11, index, 0) >= 0 && index->field_0->field_0->field_2c(index->field_0, 0, v13, &v3, &v0, &v22, &v2, 0) >= 0)
                {
                    sub_47a330(&v2, v30, &v3);
                    v31 = &v22;
                    if (v31)
                    {
                        sub_47a330(g_4cf4e8, (char *)&v3 + v30, v31);
                        v31 = &v22;
                    }
                    index->field_0->field_0->field_4c(index->field_0, &v2, &v3, g_4cf4e8, v31);
                    if (g_4d1410[index->field_8->field_4])
                    {
                        sub_46c941(g_4d1410[index->field_8->field_4]);
                        g_4d1410[index->field_8->field_4] = 0;
                    }
                    sub_454b00("Create Sound Buffer %s\n", v31);
                    _security_check_cookie();
                    return v32;
                }
LABEL_4547b9:
                if (g_4d1410[index->field_8->field_4])
                {
                    sub_46c941(g_4d1410[index->field_8->field_4]);
                    g_4d1410[index->field_8->field_4] = 0;
                }
                _security_check_cookie();
                return v33;
            }
        }
    }
    sub_464220(&g_4a3160);
    goto LABEL_4547b9;
}
