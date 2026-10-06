// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460760; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460760_1 {
    int field_0;
} st_460760_1;

typedef struct st_460760_2 {
    char padding_0[1012];
    struct st_460760_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
    unsigned int field_480;
} st_460760_2;

typedef struct st_460760_0 {
    char padding_0[8];
    struct st_460760_1 *field_8;
} st_460760_0;

extern unsigned int g_4ad138;

void sub_460760(int a0, int a1, unsigned int a2, int a3, int a4)
{
    int v5;  // ebp
    int v6;  // ebx
    st_460760_2 *idx;  // esi
    unsigned int v8;  // edx
    int v9;  // edx
    unsigned int v0;  // [bp-0xa4]
    char v1;  // [bp-0x88]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x4]
    char v4;  // [bp+0x18]

    v3 = g_4ad138 ^ &v1;
    sub_46cad8(&v1, a4, &v4, v5, v6);
    v8 = idx->field_480;
    v9 = a0;
    v0 = a2;
    sub_4606b0(idx->field_3f4->field_8->field_0, idx->field_3f4, a2, v9, a1, v8 >> 3 & 1, a3);
    idx->field_47c = idx->field_47c | 1;
    _security_check_cookie(v2 ^ &v0, v9, a1, v8 >> 3 & 1, a3);
    return;
}
