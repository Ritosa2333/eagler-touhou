// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40bd70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40bd70_0 {
    char padding_0[1212];
    unsigned int field_4bc;
    char padding_4c0[24];
    unsigned int field_4d8;
    char padding_4dc[804];
    char field_800;
} st_40bd70_0;

unsigned int sub_40bd70(unsigned int a0)
{
    unsigned short v2;  // ftop
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    st_40bd70_0 *idx;  // esi
    unsigned int v6;  // fc3210
    unsigned short v7;  // ftop
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v3 = v2 - 1;
    if (/* unsupported instruction */)
    {
        v4 = v3 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v4 = v3 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v6 = CmpF(/* unsupported instruction */, idx->field_4bc) * 0x100 & 0x4500;
        /* unsupported instruction */
        v7 = v4 + 1;
    }
    else
    {
        v6 = CmpF(nan, idx->field_4bc) * 0x100 & 0x4500;
        /* unsupported instruction */
        v7 = v4 + 1;
    }
    if ((char)(((v7 & 7) * 0x800 | (unsigned short)v6 & 0x4700) >> 8) & 65)
    {
        return 0;
    }
    else if (!(idx->field_800 & 16))
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        if (/* unsupported instruction */)
        {
            idx->field_4d8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_4d8 = nan;
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
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!/* unsupported instruction */)
        {
            idx->field_4bc = nan;
            /* unsupported instruction */
            return 1;
        }
        idx->field_4bc = /* unsupported instruction */;
        /* unsupported instruction */
        return 1;
    }
    else
    {
        return 1;
    }
}
