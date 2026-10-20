// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4800f3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4800f3_4 {
    struct st_4800f3_1 *field_0;
} st_4800f3_4;

typedef struct st_4800f3_2 {
    unsigned int field_0;
} st_4800f3_2;

typedef struct st_4800f3_1 {
    char padding_0[4];
    struct st_4800f3_2 *field_4;
} st_4800f3_1;

typedef struct st_4800f3_3 {
    struct st_4800f3_4 *field_0;
    unsigned int field_4;
} st_4800f3_3;

typedef struct st_4800f3_0 {
    char field_0;
    char field_1;
    char field_2;
} st_4800f3_0;

extern unsigned int g_4b430c;
extern unsigned int g_4b4310;
extern unsigned int g_4b4314;
extern st_4800f3_0 *g_4b4318;
extern char g_4b4330;

int sub_4800f3(void)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // ebx
    int v23;  // edx
    unsigned int v24;  // edx
    unsigned int v15;  // esi
    unsigned int v16;  // esi
    unsigned int v17;  // edi
    unsigned int v18;  // edi
    st_4800f3_3 *v19;  // eax
    int v22;  // edx
    unsigned int v0;  // [bp-0xb0]
    unsigned int v1;  // [bp-0xac]
    unsigned int v2;  // [bp-0xa8]
    unsigned int v3;  // [bp-0xa4]
    unsigned int v4;  // [bp-0x78]
    unsigned int v5;  // [bp-0x4c]
    char v6;  // [bp-0x20]
    char v7;  // [bp-0x18]
    st_4800f3_1 **v8;  // [bp-0x10]
    unsigned int v9;  // [bp-0xc]
    char v10;  // [bp-0x5]
    st_4800f3_3 *v11;  // [bp+0x4]
    char v12;  // [bp+0x8]

    if (g_4b4318->field_0 == 63 && g_4b4318->field_1 == 36)
    {
        v4 = 0xffffffff;
        v5 = 0xffffffff;
        v3 = 0xffffffff;
        v2 = v13;
        v14 = g_4b4314;
        v1 = v15;
        v16 = g_4b430c;
        g_4b430c = &v4;
        v0 = v17;
        v18 = g_4b4310;
        g_4b4310 = &v5;
        g_4b4318 = &g_4b4318->field_2;
        g_4b4314 = &v3;
        v10 = 0;
        if (g_4b4318->field_0 == 63)
        {
            g_4b4318 = &g_4b4318->field_1;
            v19 = sub_47fb56(&v7, 1, &v10);
        }
        else
        {
            v19 = sub_48023a(&v7, 1, 1);
        }
        v9 = v19->field_4;
        v8 = v19->field_0;
        if (!v19->field_0)
            g_4b4330 = 1;
        if (!v10)
        {
            sub_47e7bf(sub_47ec02(&v6, 60, sub_47f992(&v7)));
            if (v8 && (char)*(v8)->field_4() == 62)
                sub_47e9f6(&v8, v22, 32);
            sub_47e9f6(&v8, v23, 62);
            if (v12 && g_4b4318->field_0)
                g_4b4318 = &g_4b4318->field_1;
        }
        g_4b4310 = v18;
        g_4b430c = v16;
        g_4b4314 = v14;
        v11->field_0 = v8;
        v11->field_4 = v9;
        return;
    }
    sub_47e1ce(v11, v24, 2);
    return;
}
