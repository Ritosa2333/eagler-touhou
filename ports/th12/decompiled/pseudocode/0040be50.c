// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40be50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40be50_0 {
    char padding_0[1216];
    unsigned int field_4c0;
    char padding_4c4[20];
    unsigned int field_4d8;
    char padding_4dc[804];
    char field_800;
} st_40be50_0;

unsigned int sub_40be50(st_40be50_0 *idx)
{
    unsigned short v2;  // ftop

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if ((char)(((v2 & 7) * 0x800 | (unsigned short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_4c0) * 0x100 & 0x4500) & 0x4700) >> 8) & 65)
    {
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
        idx->field_4c0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 1;
    }
    else
    {
        return 1;
    }
}
