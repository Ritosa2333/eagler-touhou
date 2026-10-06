// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408b90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408b90_0 {
    char padding_0[24];
    int field_18;
    char padding_1c[36];
    int field_40;
    char padding_44[4];
    int field_48;
    char padding_4c[1224];
    unsigned int field_514;
} st_408b90_0;

unsigned int sub_408b90(void)
{
    st_408b90_0 *idx;  // esi
    int v2;  // edi
    unsigned int v3;  // edi
    int v4;  // ebx

    v3 = sub_461920(idx->field_40, v2);
    if (!v3)
        idx->field_40 = v3;
    sub_406f60(40);
    if (!v3)
    {
        return 0xffffffff;
    }
    else if (idx->field_18 >= 240)
    {
        sub_461970(idx->field_40, v4);
        sub_461970(idx->field_48);
        idx->field_40 = 0;
        sub_453d90(idx->field_48);
        return 0;
    }
    else
    {
        sub_408c30();
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
            idx->field_514 = nan;
            /* unsupported instruction */
            return 0;
        }
        idx->field_514 = /* unsupported instruction */;
        /* unsupported instruction */
        return 0;
    }
}
