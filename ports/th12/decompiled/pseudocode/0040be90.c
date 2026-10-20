// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40be90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40be90_0 {
    char padding_0[1216];
    unsigned int field_4c0;
    char padding_4c4[20];
    unsigned int field_4d8;
    char padding_4dc[804];
    char field_800;
} st_40be90_0;

unsigned int sub_40be90(st_40be90_0 *idx)
{
    unsigned short v1;  // ftop
    unsigned short v2;  // ftop
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned int v7;  // 4145

    v2 = v1 - 1;
    if (/* unsupported instruction */)
    {
        v3 = v2 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v3 = v2 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
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
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = _ccall(10, 13, (char)(((v5 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v7 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return 0;
    }
    else if (!(idx->field_800 & 16))
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
        idx->field_4d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4c0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 1;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return 1;
    }
}
