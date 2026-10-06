// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e4f1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

extern char g_49ddbc;
extern st_47dc29_1 g_4b42f8;

st_47e4f1_0 * sub_47e4f1(st_47e4f1_1 *idx, int a1, char *a2, st_47e4f1_0 *a3)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    st_47e4f1_0 *v4;  // eax
    st_47e4f1_0 *v5;  // eax
    char v6;  // 4099
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    if (idx->field_0)
        return sub_47e2f7(3);
    v0 = v3;
    if (a2)
    {
        v4 = a3;
        if (v4)
        {
            v5 = v4;
            if (v4)
            {
                if (v5 != 0x1)
                {
                    if (!sub_47dc29(&g_4b42f8.field_0, a1, 12, 0))
                        goto LABEL_47e555;
                    v5 = sub_47e3b8(a2, a3);
                }
                else
                {
                    v5 = sub_47dc29(&g_4b42f8.field_0, a1, 8, 0);
                    if (v5)
                    {
                        v6 = *(a2);
                        *((char **)&v5->padding_0[0]) = &g_49ddbc;
                        v5->field_4 = v6;
                    }
                    else
                    {
LABEL_47e555:
                        v5 = NULL;
                    }
                }
                idx->field_0 = v5;
                if (v5)
                    return v5;
            }
            idx->field_4 = 3;
            return v5;
        }
    }
    idx->field_4 = 2;
    return v4;
}
