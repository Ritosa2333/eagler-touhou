// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48023a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

typedef struct st_48023a_1 {
    st_47e7bf_0 field_0;
    unsigned int field_4;
} st_48023a_1;

typedef struct st_48023a_0 {
    char field_0;
} st_48023a_0;

typedef struct st_48023a_4 {
    unsigned int field_0;
} st_48023a_4;

extern st_48023a_4 *g_4b4310;
extern st_48023a_0 *g_4b4318;
extern unsigned int g_4b4328;
extern st_48023a_4 *g_4b432c;

int sub_48023a(void)
{
    unsigned int v16;  // ebx
    unsigned int v17;  // esi
    char *v24;  // eax
    char *v25;  // eax
    unsigned int v26;  // edx
    st_48023a_1 *v28;  // eax
    unsigned int v18;  // eax
    st_48023a_1 *v19;  // eax
    char v20;  // 4124
    int v21;  // edi
    int v22;  // edi
    int v23;  // eax
    char *v0;  // [bp-0x54]
    char *v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x44]
    unsigned int v4;  // [bp-0x40]
    char v5;  // [bp-0x3c]
    st_48023a_1 *v6;  // [bp-0x34]
    char v7;  // [bp-0x30]
    unsigned int v8;  // [bp-0x2c]
    struct st_47e213_1 *v9[2];  // [bp-0x28]
    st_47e7bf_0 v10;  // [bp-0x20]
    unsigned int v11;  // [bp-0x1c]
    char v12[16];  // [bp-0x18]
    st_48023a_1 *v13;  // [bp+0x4]
    char v14;  // [bp+0x8]
    char v15;  // [bp+0xc]

    v4 = v16;
    v3 = v17;
    v18 = g_4b4318->field_0 - 48;
    v6 = v13;
    if (v18 <= 9)
    {
        g_4b4318 = g_4b4318 + 1;
        sub_47e378(v13, v18);
        return;
    }
    v10 = (st_47e7bf_0)0;
    v11 &= 0xffff0000;
    if (g_4b4318->field_0 == 63)
    {
        v19 = sub_4800f3(v9, 0);
        v11 = *((int *)&v19->field_0.field_4);
        v10 = (st_47e7bf_0)v19->field_0.field_0;
        v20 = g_4b4318->field_0;
        g_4b4318 = g_4b4318 + 1;
        if (v20 != 64)
        {
            g_4b4318 = (char *)g_4b4318 - 1;
            sub_47e2f7(&v10, v26, (unsigned int)((g_4b4318->field_0) + 1));
        }
    }
    else
    {
        v21 = "template-parameter-";
        if (!sub_47e029(19, v22))
        {
            g_4b4318 = g_4b4318 + 19;
            goto LABEL_480322;
        }
        else
        {
            v21 = "generic-type-";
            if (!sub_47e029(13))
            {
                g_4b4318 = g_4b4318 + 13;
LABEL_480322:
                sub_47f57e(v9);
                if (g_4b4328 & 0x4000)
                {
                    sub_47e213(v9, v26, v12, 16);
                    v23 = sub_46d114(v12);
                    v24 = g_4b432c(v23);
                    if (v24)
                    {
                        sub_47e896(&v10, v26, v24);
                    }
                    else
                    {
                        sub_47e896(&v10, v26, "`");
                        v2 = "'";
                        v1 = &v5;
                        v0 = v9;
                        v25 = &v7;
LABEL_4803a3:
                        sub_47ec4a(v25, v21);
                        sub_47e7bf(&v10, v26, sub_47ec92());
                    }
                }
                else
                {
                    sub_47e896(&v10, v26, "`");
                    v2 = "'";
                    v1 = &v7;
                    v0 = v9;
                    v25 = &v5;
                    goto LABEL_4803a3;
                }
            }
            else if (v15 && g_4b4318->field_0 == 64)
            {
                g_4b4318 = g_4b4318 + 1;
                v10 = (st_47e7bf_0)0;
                v11 = v8 & 0xffff0000;
            }
            else
            {
                v28 = sub_47e5d3(&g_4b4318, 64);
                v10 = (st_47e7bf_0)v28->field_0.field_0;
                v11 = *((int *)&v28->field_0.field_4);
            }
        }
    }
    if (v14 && g_4b4310->field_0 != 9)
        sub_47e32e(&v10);
    v6->field_0 = v10;
    *((unsigned int *)&v6->field_0.field_4) = v11;
    return;
}
