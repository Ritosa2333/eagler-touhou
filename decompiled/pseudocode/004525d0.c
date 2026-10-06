// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4525d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4525d0_0 {
    char padding_0[24];
    unsigned int field_18;
    int field_1c;
    int field_20;
    char padding_24[3];
    char field_27;
} st_4525d0_0;

extern unsigned int g_4ce55c;

unsigned int sub_4525d0(st_4525d0_0 *idx)
{
    unsigned int v5;  // esi
    unsigned int v6;  // ecx
    unsigned int v7;  // edi
    unsigned int v8;  // fpround
    unsigned int v9;  // 4223
    unsigned int v10;  // eax
    unsigned int v11;  // edi
    int v12;  // 4101
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    int v2;  // [bp-0xc], Other Possible Types: unsigned short
    unsigned long v3;  // [bp-0x8], Other Possible Types: unsigned int

    v1 = v5;
    v6 = idx->field_27;
    if (g_4ce55c)
        return 0;
    v2 = idx->field_1c;
    if (*((int *)&idx[1].padding_0[12]) < idx->field_1c)
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
        v3 = v6;
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
        v0 = v7;
        if (v3 < 0)
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
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = _ccall(v8);
        v2 = v9;
        v3 = v2 | 0xc00;
        v10 = v6;
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v11 = v3;
        idx->field_18 = v10 - v11;
        if (v10 - v11 < 0)
        {
            idx->field_18 = 0;
            sub_464a80();
            return 1;
        }
    }
    else
    {
        idx->field_20 = idx->field_20 - 1;
        v12 = idx->field_20;
        idx->field_18 = 0;
        if (v12 <= 0)
            return 0;
        sub_4067e0(0);
    }
    sub_464a80();
    return 1;
}
