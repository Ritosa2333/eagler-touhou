// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f992; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

typedef struct st_47f992_0 {
    char field_0;
} st_47f992_0;

typedef struct st_47f992_1 {
    unsigned int field_0;
} st_47f992_1;

extern st_47f992_1 *g_4b4314;
extern st_47f992_0 *g_4b4318;
extern unsigned int g_4b4328;
extern st_47f992_1 *g_4b432c;
extern char g_4b4331;

int sub_47f992(st_47e7bf_0 *idx)
{
    unsigned int v22;  // ebx
    unsigned int v23;  // esi
    int v31;  // eax
    char *v32;  // eax
    unsigned int v35;  // edx
    unsigned int v33;  // edx
    unsigned int v34;  // eax
    unsigned int v24;  // edi
    char *v25;  // eax
    unsigned int v26;  // edx
    unsigned int v27;  // ecx
    st_47e7bf_0 *v28;  // eax
    char *v29;  // eax
    unsigned int *v30;  // eax
    char *v0;  // [bp-0x8c]
    char *v1;  // [bp-0x88]
    char *v2;  // [bp-0x84], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x7c]
    unsigned int v4;  // [bp-0x78]
    unsigned int v5;  // [bp-0x74]
    char v6;  // [bp-0x70]
    char v7;  // [bp-0x68]
    char v8;  // [bp-0x60]
    char v9;  // [bp-0x58]
    char v10;  // [bp-0x50]
    char v11;  // [bp-0x48]
    char v12;  // [bp-0x40]
    char *v13;  // [bp-0x38]
    unsigned int v14;  // [bp-0x34]
    unsigned int v15;  // [bp-0x34]
    unsigned int v16;  // [bp-0x30]
    struct st_47e213_1 *v17[2];  // [bp-0x2c]
    unsigned int v18;  // [bp-0x24]
    unsigned int v19;  // [bp-0x20]
    unsigned int v20;  // [bp-0x1c]
    char v21[16];  // [bp-0x18]

    v5 = v22;
    v4 = v23;
    idx->field_4 = 0;
    *((unsigned int *)&idx->field_4) = *((int *)&idx->field_4) & 0xffff00ff;
    idx->field_0 = 0;
    g_4b4331 = 1;
    v18 = 1;
    if (!idx->field_4)
    {
        v3 = v24;
        do
        {
            v25 = &g_4b4318->field_0;
            if (!g_4b4318->field_0 || g_4b4318->field_0 == 64)
                break;
            if (v18)
            {
                v18 = 0;
            }
            else
            {
                sub_47e9f6(44);
                v25 = &g_4b4318->field_0;
            }
            v26 = _INSERT(0, 0, *(v25));
            v27 = *(v25) - 48;
            if (v27 <= 9)
            {
                g_4b4318 = v25 + 1;
                v28 = sub_47e378(&v6, v27);
                continue;
            }
            v20 &= 0xffff0000;
            v13 = v25;
            v19 = 0;
            if ((char)v26 == 88)
            {
                g_4b4318 = v25 + 1;
                v2 = "void";
                goto LABEL_47fa45;
            }
            else
            {
                if ((char)v26 == 36 && (v29 = v25 + 1, *(v29) != (char)v26))
                {
                    g_4b4318 = v29;
                    v30 = sub_47f5d0(&v12);
                    goto LABEL_47fafe;
                }
                if ((char)v26 == 63)
                {
                    sub_47f57e(v17);
                    if (g_4b4328 & 0x4000)
                    {
                        sub_47e213(v17, v35, v21, 16);
                        v31 = sub_46d114(v21);
                        v2 = g_4b432c(v31);
                        if (v2)
                        {
                            v2 = v2;
                        }
                        else
                        {
                            v2 = "'";
                            v1 = &v7;
                            v0 = v17;
                            v32 = &v9;
                            goto LABEL_47fac1;
                        }
                    }
                    else
                    {
                        v2 = "'";
                        v1 = &v11;
                        v0 = v17;
                        v32 = &v10;
LABEL_47fac1:
                        sub_47ec4a(v32, "`template-parameter");
                        v30 = sub_47ec92();
                        goto LABEL_47faff;
                    }
LABEL_47fa45:
                    sub_47e896(&v19, v26, v2);
                    goto LABEL_47fb0a;
                }
                else
                {
                    v16 &= 0xffff0000;
                    v14 = 0;
                    v30 = sub_4826a7(&v8, &v14);
                    v15 = v14;
LABEL_47fafe:
LABEL_47faff:
                    v20 = v30[1];
                    v19 = *(v30);
LABEL_47fb0a:
                    if (g_4b4318 - v13 > 1 && g_4b4314->field_0 != 9)
                        sub_47e32e(&v19);
                    v28 = &v19;
                }
            }
            sub_47e7bf(idx, v33, v28);
        } while (!idx->field_4);
    }
    g_4b4331 = 0;
    return v34;
}
