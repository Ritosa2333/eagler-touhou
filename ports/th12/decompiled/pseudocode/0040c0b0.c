// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40c0b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40c0b0_0 {
    char padding_0[1216];
    unsigned int field_4c0;
    char padding_4c4[100];
    unsigned int field_528;
    char padding_52c[24];
    int field_544;
    char padding_548[808];
    int field_870;
} st_40c0b0_0;

unsigned int sub_40c0b0(void)
{
    unsigned int v5;  // ftop
    unsigned int v6;  // ftop
    unsigned short v16;  // ftop
    unsigned short v17;  // ftop
    unsigned short v18;  // ftop
    unsigned short v19;  // ftop
    unsigned int v21;  // 4145
    unsigned int v22;  // ecx
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned int v13;  // esi
    st_40c0b0_0 *idx;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
        v8 = v7 + 1;
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
        v8 = v7 + 1;
    }
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
    if (!sub_409970())
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = v13;
    /* unsupported instruction */
    if (!((char)(((v11 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_4c0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_40c0f8;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_40c0f8;
        }
    }
    else
    {
        v16 = v11 - 1;
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
        v18 = v17 - 1;
        if (/* unsupported instruction */)
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v21 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v21 & 1))
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_40c0f8:
            if (/* unsupported instruction */)
            {
                idx->field_4c0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx->field_4c0 = nan;
                /* unsupported instruction */
            }
            v0 = v22;
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
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
            }
            sub_464a20();
            if (idx->field_544 >= 0)
                sub_453d90();
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    if (idx->field_870 > 0)
        return 0;
    idx->field_528 = idx->field_528 ^ 0x40000;
    return 1;
}
