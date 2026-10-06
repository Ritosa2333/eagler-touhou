// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43aa50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43aa50_0 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[24];
    unsigned int field_48;
    int field_4c;
} st_43aa50_0;

unsigned int sub_43aa50(unsigned int a0, st_43aa50_0 *idx)
{
    unsigned int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v10;  // fc3210
    unsigned short v11;  // ftop
    unsigned int v12;  // 4144
    st_43aa50_0 *v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

    v6 = v8;
    v5 = v9;
    if (idx->field_48 == 2)
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_2c) * 0x100 & 0x4500;
    /* unsupported instruction */
    v12 = _ccall(10, 13, (char)(((v11 & 7) * 0x800 | (unsigned short)v10 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v12 & 1))
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
        if (!/* unsupported instruction */)
        {
            idx->field_2c = nan;
            /* unsupported instruction */
            return 0;
        }
        idx->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
        return 0;
    }
    else
    {
        idx->field_48 = 2;
        sub_461970(idx->field_4c);
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
        v4 = 4;
        v3 = 15;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
        }
        v0 = &idx->padding_0[20];
        sub_4390f0();
        return 0;
    }
}
