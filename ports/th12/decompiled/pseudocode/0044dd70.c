// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44dd70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44dd70_0 {
    struct st_44dd70_0 *field_0;
} st_44dd70_0;

typedef struct st_44d420_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[4];
    char field_10;
    char padding_11[15];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
} st_44d420_0;

extern unsigned int g_4ad138;

char sub_44dd70(unsigned int a0, unsigned int a1, int a2, int a3)
{
    unsigned int v12;  // edi
    st_44dd70_0 **v13;  // eax
    int v14;  // ebx
    char v15;  // al
    unsigned int v0;  // [bp-0x44]
    unsigned int v1;  // [bp-0x38]
    st_44d420_0 v2;  // [bp-0x30]
    int v3;  // [bp-0x2c]
    int v4;  // [bp-0x28]
    st_44dd70_0 **v5;  // [bp-0x20]
    unsigned int v6;  // [bp-0xc]
    int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]
    int v9;  // [bp+0xc]
    int v10;  // [bp+0x10]
    int v11;  // [bp+0x14]

    v8 = g_4ad138 ^ &v2;
    v1 = v12;
    sub_44d420(&v2);
    v3 = 0;
    v4 = v9;
    if (v2)
    {
        DeleteObject(v2);
        v2 = (st_44d420_0)0;
    }
    v13 = v5;
    if (v6 < 0x10)
        v13 = &v5;
    v2 = (st_44d420_0)CreateFontA(v4, v3, 0, 0, v7, 0, 0, 0, 1, 0, 0, 0, 48, v13);
    sub_44de50(a2, a3, v2, v10, v11);
    if (v2)
    {
        DeleteObject(v2);
        v0 = 0;
    }
    if (v5 >= 0x10)
        sub_46ca4f(v14);
    _security_check_cookie();
    return v15;
}
