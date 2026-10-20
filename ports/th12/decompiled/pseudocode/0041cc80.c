// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41cc80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41cc80_0 {
    char padding_0[102272];
    unsigned int field_18f80;
} st_41cc80_0;

extern st_41cc80_0 *g_4b43b8;
extern unsigned int g_4cee40;

unsigned int sub_41cc80(unsigned int a0)
{
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v16;  // ftop
    unsigned short v17;  // ftop
    unsigned int v19;  // ebx
    unsigned short v8;  // ftop
    unsigned int v9;  // esi
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v12;  // ftop
    unsigned int v14;  // 4116
    unsigned int v15;  // eax
    unsigned long v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    if (g_4cee40 == 15)
    {
        return 1;
    }
    else if (g_4cee40 != 4)
    {
        v7 = v6 - 1;
        if (/* unsupported instruction */)
        {
            v8 = v7 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v8 = v7 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v2 = v9;
        if (/* unsupported instruction */)
        {
            a0 = /* unsupported instruction */;
            /* unsupported instruction */
            v10 = v8 + 1;
        }
        else
        {
            a0 = nan;
            /* unsupported instruction */
            v10 = v8 + 1;
        }
        if (!g_4b43b8)
            return 1;
        v11 = v10 - 1;
        if (/* unsupported instruction */)
        {
            v12 = v11 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v12 = v11 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v14 = _ccall(10, 13, (char)(((v12 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, 0x403e000000000000) * 0x100 & 0x4500 : CmpF(nan, 0x403e000000000000) * 0x100 & 0x4500) & 0x4700) >> 8) & 5, 0, 0);
        if (!(v14 & 1))
        {
            v15 = 0xff5050ff;
        }
        else
        {
            v16 = v12 - 1;
            if (/* unsupported instruction */)
            {
                v17 = v16 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v17 = v16 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v15 = (!((char)(((v17 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65) ? 0xffa0a0ff : 0xffffffff);
        }
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
        v1 = v19;
        if (/* unsupported instruction */)
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v3 = nan;
            /* unsupported instruction */
        }
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
        if (/* unsupported instruction */)
        {
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        g_4b43b8->field_18f80 = v15;
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_401720("%2.1ffps");
        g_4b43b8->field_18f80 = 0xffffffff;
        return 1;
    }
    else
    {
        return 1;
    }
}
