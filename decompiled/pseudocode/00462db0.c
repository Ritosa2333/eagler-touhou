// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462db0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462db0_1 {
    unsigned int field_0;
} st_462db0_1;

typedef struct st_462db0_0 {
    char padding_0[28];
    struct st_462db0_1 *field_1c;
    char padding_20[4];
    struct st_462db0_1 *field_24;
    char padding_28[60];
    struct st_462db0_1 *field_64;
} st_462db0_0;

typedef struct st_462db0_4 {
    struct st_462db0_0 *field_0;
} st_462db0_4;

extern unsigned int g_4ad138;
extern st_462db0_4 *g_4ce90c;
extern unsigned int g_4cee78;
extern unsigned int g_4d4eb0;

void sub_462db0(void)
{
    int v7;  // edi
    int v8;  // esi
    unsigned int node;  // eax
    unsigned int v10;  // ecx
    int v11;  // esi
    unsigned int v12;  // eax
    unsigned int v13;  // ecx
    unsigned int *i;  // esi
    unsigned int *iter;  // edi
    char v0;  // [bp-0x154]
    unsigned int v1[13];  // [bp-0x150]
    unsigned int v2[13];  // [bp-0x150]
    unsigned int v3;  // [bp-0x14c]
    char v4;  // [bp-0xfc]
    unsigned int v5;  // [bp-0x8]

    v5 = g_4ad138 ^ &v0;
    sub_477420(&g_4d4eb0, 0, 128, v7, v8);
    if (!(g_4cee78 & 0x800))
    {
        sub_477420(v1, 0, 52);
        v2 = (unsigned int (32 bits)[13])52;
        v3 = 0xff;
        node = joyGetPosEx(0, v2);
        if (!node)
        {
            v10 = *(&v1[8]);
            do
            {
                if ((char)v10 & 1)
                    *(node + (char *)&g_4d4eb0) = 128;
            } while ((node += 1, v10 >>= 1, node < 32));
        }
    }
    else if (g_4ce90c->field_0->field_64(g_4ce90c) < 0)
    {
        v11 = 0;
        if (g_4ce90c->field_0->field_1c(g_4ce90c) == 2147942430)
        {
            do
            {
                v12 = g_4ce90c->field_0->field_1c(g_4ce90c);
                v11 += 1;
            } while (v11 < 400 && v12 == 2147942430);
        }
    }
    else
    {
        g_4ce90c->field_0->field_24(g_4ce90c, 272, &v1[12]);
        v13 = 32;
        i = &v4;
        for (iter = &g_4d4eb0; v13; i += 1)
        {
            v13 -= 1;
            *(iter) = *(i);
            iter += 1;
        }
    }
    _security_check_cookie();
    return;
}
