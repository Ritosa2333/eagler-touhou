// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462a80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462a80_1 {
    unsigned int field_0;
} st_462a80_1;

typedef struct st_462a80_0 {
    char padding_0[28];
    struct st_462a80_1 *field_1c;
    char padding_20[4];
    struct st_462a80_1 *field_24;
    char padding_28[60];
    struct st_462a80_1 *field_64;
} st_462a80_0;

typedef struct st_462a80_4 {
    struct st_462a80_0 *field_0;
} st_462a80_4;

extern st_462a80_4 *g_4ce90c;
extern unsigned int g_4cee78;

int sub_462a80(unsigned int a0)
{
    unsigned int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    unsigned int v11;  // eax
    int v12;  // edi
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x15c]
    unsigned int v1;  // [bp-0x158]
    unsigned int v2;  // [bp-0x154]
    unsigned int v3[13];  // [bp-0x150]
    unsigned int v4[13];  // [bp-0x150]
    unsigned int v5;  // [bp-0x14c]
    char v6;  // [bp-0x120]

    v2 = v8;
    v1 = v9;
    v0 = v10;
    if (!(g_4cee78 & 0x800))
    {
        sub_477420(v3, 0, 52);
        v4 = (unsigned int (32 bits)[13])52;
        v5 = 0xff;
        if (!joyGetPosEx(0, v4))
            return v11;
    }
    else if (g_4ce90c->field_0->field_64(g_4ce90c) < 0)
    {
        v12 = 0;
        if (g_4ce90c->field_0->field_1c(g_4ce90c) == 2147942430)
        {
            while (1)
            {
                g_4ce90c->field_0->field_1c(g_4ce90c);
                v13 = sub_4654b0("error : DIERR_INPUTLOST %d\r\n", v12);
                v12 += 1;
                if (v12 >= 400)
                    break;
                if (v13 != 2147942430)
                {
                    _security_check_cookie();
                    return v14;
                }
            }
        }
    }
    else
    {
        sub_477420(&v6, 0, 272);
        if (g_4ce90c->field_0->field_24(g_4ce90c, 272, &v6) >= 0)
        {
            _security_check_cookie();
            return v16;
        }
    }
    _security_check_cookie();
    return v15;
}
