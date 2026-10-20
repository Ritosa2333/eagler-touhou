// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x480514; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

typedef struct st_480514_1 {
    st_47e7bf_0 field_0;
    unsigned int field_4;
} st_480514_1;

typedef struct st_480514_0 {
    char field_0;
} st_480514_0;

extern st_480514_0 *g_4b4318;
extern unsigned int g_4b4328;

int sub_480514(void)
{
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    st_480514_1 *v19;  // eax
    unsigned int v20;  // edx
    unsigned int v11;  // ecx
    unsigned int v12;  // edi
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    char *v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    char v3;  // [bp-0x1c]
    st_47e7bf_0 v4;  // [bp-0x14], Other Possible Types: char
    unsigned int v5;  // [bp-0x10]
    st_47e7bf_0 v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    st_480514_1 *v8;  // [bp+0x4]

    v2 = v9;
    v1 = v10;
    v11 = ~(g_4b4328 >> 15);
    v12 = 1;
    if (!(v11 & 1) || g_4b4328 & 0x1000)
        v12 = 0;
    v13 = g_4b4318->field_0;
    v6 = (st_47e7bf_0)0;
    v7 &= 0xffff0000;
    g_4b4318 = g_4b4318 + 1;
    v14 = v13;
    if (!v14)
    {
        g_4b4318 = (char *)g_4b4318 - 1;
        sub_47e59a("unknown ecsu'");
        return;
    }
    v15 = v14 - 84;
    if (v14 != 84)
    {
        v16 = v15 - 1;
        if (v15 != 1)
        {
            v17 = v16 - 1;
            if (v16 != 1)
            {
                v18 = v17 - 1;
                if (v17 == 1)
                {
                    v12 = v11 & 1;
                    v19 = sub_47ec4a(&v3, "enum ", sub_47ee08(&v4));
                    v6 = (st_47e7bf_0)v19->field_0.field_0;
                    v7 = *((int *)&v19->field_0.field_4);
                    goto LABEL_4805c4;
                }
                else if (v18 != 1)
                {
                    if (v18 != 2)
                        goto LABEL_4805c4;
                    v0 = "cointerface ";
                }
                else
                {
                    v0 = "coclass ";
                }
            }
            else
            {
                v0 = "class ";
            }
        }
        else
        {
            v0 = "struct ";
        }
    }
    else
    {
        v0 = "union ";
    }
    sub_47e896(&v6, v20, v0);
LABEL_4805c4:
    v4 = (st_47e7bf_0)0;
    v5 &= 0xffff0000;
    if (v12)
    {
        v4 = v6;
        v5 = v7;
    }
    sub_480430(&v6);
    sub_47e7bf(&v4, v20, &v6);
    v8->field_0 = v4;
    *((unsigned int *)&v8->field_0.field_4) = v5;
    return;
}
