// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40c230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40c230_0 {
    char padding_0[1212];
    unsigned int field_4bc;
    unsigned int field_4c0;
    unsigned int field_4c4;
    unsigned int field_4c8;
    unsigned int field_4cc;
    unsigned int field_4d0;
    unsigned int field_4d4;
    unsigned int field_4d8;
    char padding_4dc[76];
    unsigned int field_528;
    char padding_52c[940];
    int field_8d8;
    char padding_8dc[20];
    unsigned int field_8f0;
    unsigned int field_8f4;
    unsigned int field_8f8;
} st_40c230_0;

int sub_40c230(void)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // esi
    unsigned int v23;  // 4367
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    unsigned int v15;  // edi
    st_40c230_0 *idx;  // eax
    unsigned int v17;  // ecx
    unsigned int v18;  // edx
    unsigned int v19;  // eax
    unsigned short v20;  // ftop
    unsigned short v21;  // ftop
    unsigned int v0;  // [bp-0x44]
    unsigned int v1;  // [bp-0x40]
    unsigned int v2;  // [bp-0x3c]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x2c]
    unsigned int v6;  // [bp-0x28]
    unsigned int v7;  // [bp-0x24]
    unsigned int v8;  // [bp-0x20]
    unsigned int v9;  // [bp-0x1c]
    unsigned int v10;  // [bp-0x18]
    unsigned int v11;  // [bp-0x14]

    v4 = v13;
    v3 = v14;
    v2 = v15;
    if (idx->field_8d8 >= *((int *)&idx[1].padding_0[0]))
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
        v17 = idx->field_8f0;
        v18 = idx->field_8f4;
        if (/* unsupported instruction */)
        {
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v5 = nan;
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
        v19 = idx->field_8f8;
        idx->field_528 = idx->field_528 & 0xfdffffff;
        if (/* unsupported instruction */)
            idx->field_4d4 = /* unsupported instruction */;
        else
            idx->field_4d4 = nan;
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
        }
        idx->field_4bc = v17;
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
        idx->field_4c0 = v18;
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
        idx->field_4c4 = v19;
        sub_40d640();
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return;
    }
    else
    {
        v6 = idx->field_4bc;
        v7 = idx->field_4c0;
        v8 = idx->field_4c4;
        sub_405900();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4c8 = v9;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4cc = v10;
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_4d0 = v11;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v21 = v20 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = _ccall(10, 13, (char)(((v21 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v23 & 1)
        {
            v24 = v21 - 0;
            if (/* unsupported instruction */)
            {
                v25 = v24 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v25 = v24 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)(((v25 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_4d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_464a80();
                return;
            }
        }
        else
        {
            /* unsupported instruction */
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
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        sub_4937aa();
        if (/* unsupported instruction */)
        {
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v5 = nan;
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
            idx->field_4d8 = nan;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_4d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_464a80();
            return;
        }
        idx->field_4d8 = /* unsupported instruction */;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4d0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_464a80();
        return;
    }
}
