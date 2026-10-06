// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x437980; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_437980_0 {
    char padding_0[27952];
    unsigned int field_6d30;
} st_437980_0;

typedef struct st_437980_1 {
    char padding_0[2600];
    unsigned int field_a28;
    char padding_a2c[47592];
    char field_c414;
} st_437980_1;

extern st_437980_0 *g_4b43e4;
extern st_437980_1 *g_4b4514;

int sub_437980(void)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // ftop
    unsigned short v13;  // ftop
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned int v17;  // 4176
    unsigned int v18;  // eax
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v9;  // ftop
    unsigned int v10;  // 4393
    unsigned short v11;  // ftop
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    v4 = v3 - 1;
    if (/* unsupported instruction */)
    {
        v5 = v4 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v5 = v4 - 1;
        /* unsupported instruction */
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = v7 - 1;
    v10 = _ccall(10, 13, (char)(((v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (!(v10 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v11 = v9 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v11 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = v11 + 1;
        }
        v14 = v13 - 1;
        if (/* unsupported instruction */)
        {
            v15 = v14 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v15 = v14 - 1;
            /* unsupported instruction */
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v17 = _ccall(10, 13, (char)(((v15 + 3 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        return;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (g_4b43e4 && g_4b43e4->field_6d30)
            return;
        v18 = g_4b4514->field_a28;
        if (v18 == 2)
        {
            return;
        }
        else if (v18 == 4)
        {
            return;
        }
        else if (v18 == 3)
        {
            return;
        }
        else if (g_4b4514->field_c414 & 2)
        {
            return;
        }
        else
        {
            if (*((int *)&g_4b4514->padding_a2c[47576]) > NULL)
                return;
            sub_438370();
            return;
        }
    }
}
