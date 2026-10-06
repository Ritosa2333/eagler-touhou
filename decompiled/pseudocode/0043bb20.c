// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43bb20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43bb20_0 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[440];
    char field_1cc;
} st_43bb20_0;

typedef struct st_43bb20_1 {
    char padding_0[102272];
    unsigned int field_18f80;
} st_43bb20_1;

extern st_43bb20_1 *g_4b43b8;
extern unsigned int g_4b44e8;

unsigned int sub_43bb20(st_43bb20_0 *a0)
{
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned int v15;  // fc3210
    unsigned short v16;  // ftop
    unsigned int v17;  // 4116
    int v18;  // esi
    int v19;  // ebx
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v13;  // ftop
    unsigned int v14;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    if (!g_4b44e8)
    {
        return 1;
    }
    else if (!a0->field_10)
    {
        return 1;
    }
    else if (a0->field_10 == 1)
    {
        v6 = v5 - 1;
        if (/* unsupported instruction */)
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
            v8 = v7 + 1;
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
            v8 = v7 + 1;
        }
        v0 = a0->field_1cc;
        v9 = v8 - 1;
        if (/* unsupported instruction */)
        {
            v10 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v10 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
            v11 = v10 + 1;
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
            v11 = v10 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = v11 - 1;
        if (!((char)(((v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = 0xff5050ff;
        }
        else
        {
            if (/* unsupported instruction */)
            {
                v15 = CmpF(/* unsupported instruction */, 0x4049000000000000) * 0x100 & 0x4500;
                /* unsupported instruction */
                v16 = v13 + 1;
            }
            else
            {
                v15 = CmpF(nan, 0x4049000000000000) * 0x100 & 0x4500;
                /* unsupported instruction */
                v16 = v13 + 1;
            }
            v17 = _ccall(10, 13, (char)(((v16 & 7) * 0x800 | (unsigned short)v15 & 0x4700) >> 8) & 5, 0, 0);
            v14 = (v17 & 1 ? 0xffffffff : 0xffa0a0ff);
        }
        g_4b43b8->field_18f80 = v14;
        sub_4015c0("%3d", a0->field_1cc, v18, v19);
        g_4b43b8->field_18f80 = 0xffffffff;
        return 1;
    }
    else
    {
        return 1;
    }
}
