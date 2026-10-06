// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460800_1 {
    int field_0;
} st_460800_1;

typedef struct st_460800_2 {
    char padding_0[1012];
    struct st_460800_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
    unsigned int field_480;
    char padding_484[24];
    char field_49c;
} st_460800_2;

typedef struct st_460800_0 {
    char padding_0[8];
    struct st_460800_1 *field_8;
} st_460800_0;

extern unsigned int g_4ad138;

void sub_460800(int a0, int a1, unsigned int a2, int a3, int a4)
{
    unsigned int v6;  // ebx
    unsigned int v7;  // edi
    unsigned int v8;  // ebp
    st_460800_2 *idx;  // esi
    char *v10;  // eax
    char *v11;  // eax
    char *v12;  // eax
    int v13;  // eax
    unsigned int v0;  // [bp-0x98]
    unsigned int v1;  // [bp-0x90]
    int <0x460800[is_1]|Stack bp-0x8c, 1 B>;  // [bp-0x8c]
    unsigned int v2;  // [bp-0x8c]
    char v3;  // [bp-0x88]
    unsigned int v4;  // [bp-0x4]
    char v5;  // [bp+0x18]

    v4 = g_4ad138 ^ &<0x460800[is_1]|Stack bp-0x8c, 1 B>;
    v1 = v6;
    v0 = v7;
    v8 = 0x11;
    if (idx->field_49c > 0)
        v8 = idx->field_49c;
    sub_46cad8(&v3, a4, &v5);
    v10 = &v3;
    do
    {
        v11 = v10;
        v12 = v11 + 1;
        v10 = v12;
    } while (*(v11));
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
    v2 = (v12 - (&v3 + 1)) * (v8 - 1) >> 1;
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
    if (v2 < 0)
    {
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
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v13 = sub_4931e0(a0, a1, idx->field_480 >> 3 & 1, a3);
    sub_4606b0(idx->field_3f4->field_8->field_0, idx->field_3f4, v13);
    idx->field_47c = idx->field_47c | 1;
    _security_check_cookie();
    return;
}
