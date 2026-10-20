// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4608f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4608f0_1 {
    int field_0;
} st_4608f0_1;

typedef struct st_4608f0_2 {
    char padding_0[1012];
    struct st_4608f0_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
    unsigned int field_480;
    char padding_484[24];
    char field_49c;
} st_4608f0_2;

typedef struct st_4608f0_0 {
    char padding_0[8];
    struct st_4608f0_1 *field_8;
} st_4608f0_0;

extern unsigned int g_4ad138;

void sub_4608f0(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, int a4)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // edi
    unsigned int v12;  // ebp
    st_4608f0_2 *idx;  // esi
    char *v14;  // eax
    char *v15;  // eax
    char *v16;  // eax
    int v17;  // eax
    unsigned int v0;  // [bp-0xa8]
    unsigned int v1;  // [bp-0xa4]
    unsigned int v2;  // [bp-0xa0]
    unsigned int v3;  // [bp-0x9c]
    unsigned int v4;  // [bp-0x98]
    unsigned int v5;  // [bp-0x90]
    char v6;  // [bp-0x8c]
    char v7;  // [bp-0x88]
    unsigned int v8;  // [bp-0x4]
    char v9;  // [bp+0x18]

    v8 = g_4ad138 ^ &v6;
    v5 = v10;
    v4 = v11;
    v12 = 0x11;
    if (idx->field_49c > 0)
        v12 = idx->field_49c;
    sub_46cad8(&v7, a4, &v9);
    v14 = &v7;
    do
    {
        v15 = v14;
        v16 = v15 + 1;
        v14 = v16;
    } while (*(v15));
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v3 = a3;
    v2 = idx->field_480 >> 3 & 1;
    v1 = a1;
    v0 = a0;
    v17 = sub_4931e0();
    sub_4606b0(idx->field_3f4->field_8->field_0, idx->field_3f4, v17 / 2 - ((v16 - (&v7 + 1)) * (v12 - 1) >> 2));
    idx->field_47c = idx->field_47c | 1;
    _security_check_cookie();
    return;
}
