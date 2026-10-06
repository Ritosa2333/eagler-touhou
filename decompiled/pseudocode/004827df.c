// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4827df; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de46_0 {
    char padding_0[4];
    char field_4;
} st_47de46_0;

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

typedef struct st_481a74_1 {
    st_47e26b_0 field_0;
    unsigned int field_4;
} st_481a74_1;

typedef struct st_4827df_0 {
    char field_0;
} st_4827df_0;

extern st_4827df_0 *g_4b4318;

int sub_4827df(void)
{
    unsigned int v7;  // edx
    unsigned int *v8;  // eax
    unsigned int v0[2];  // [bp-0x1c]
    st_481a74_1 v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    st_481a74_1 v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp+0x4]
    st_47de46_0 *v6;  // [bp+0x8]

    sub_47e17b(&v3, v7, v6);
    if (!g_4b4318->field_0)
    {
        sub_47ec26(v5, 1, &v3);
    }
    else if (g_4b4318->field_0 != 63)
    {
        if (g_4b4318->field_0 != 88)
        {
            sub_4826a7(v5, &v3);
            return;
        }
        g_4b4318 = g_4b4318 + 1;
        if (!v3)
        {
            sub_47e59a(v5, v7, "void");
            return;
        }
        sub_47ec4a(v5, "void ", &v3);
    }
    else
    {
        g_4b4318 = g_4b4318 + 1;
        v2 &= 0xffff0000;
        v1 = (st_481a74_1)0;
        v8 = sub_481a74(v0, &v3, 0, &v1, 0);
        v3 = (st_481a74_1)*(v8);
        v4 = v8[1];
        sub_4826a7(v5, &v3);
        return;
    }
    return;
}
