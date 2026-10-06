// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464a20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464a20_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    struct st_464a20_1 *field_c;
} st_464a20_0;

typedef struct st_464a20_1 {
    unsigned int field_0;
} st_464a20_1;


unsigned int sub_464a20(unsigned int a0)
{
    st_464a20_0 *idx;  // esi
    unsigned int *v1;  // ecx
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned int v7;  // eax
    unsigned int v8;  // eax

    v1 = &idx->field_c->field_0;
    idx->field_0 = idx->field_4;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(((v3 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
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
        /* unsupported instruction */
        if (!((char)(((v5 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v1)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
                idx->field_8 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx->field_8 = nan;
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
            v7 = sub_4931e0();
            idx->field_4 = v7;
            return v7;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    a0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = sub_4931e0();
    idx->field_4 = v8;
    return v8;
}
