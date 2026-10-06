// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464a80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464a80_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    struct st_464a80_1 *field_c;
} st_464a80_0;

typedef struct st_464a80_1 {
    unsigned int field_0;
} st_464a80_1;


unsigned int sub_464a80(unsigned int a0)
{
    st_464a80_0 *idx;  // esi
    unsigned int v3;  // edx
    unsigned int *v4;  // ecx
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned int v10;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v3 = idx->field_4;
    v4 = &idx->field_c->field_0;
    idx->field_0 = v3;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(((v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
        /* unsupported instruction */
        if (!((char)(((v8 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v4)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
            idx->field_4 = v3 + 1;
            if (/* unsupported instruction */)
            {
                idx->field_8 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx->field_8 = nan;
                /* unsupported instruction */
            }
            return v3 + 1;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = sub_4931e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    idx->field_4 = v10;
    return v10;
}
