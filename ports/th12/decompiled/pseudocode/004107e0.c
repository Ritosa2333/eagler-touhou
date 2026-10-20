// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4107e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4107e0_0 {
    char padding_0[12];
    int field_c;
    char padding_10[16];
    unsigned int field_20;
    char padding_24[7];
    char field_2b;
} st_4107e0_0;

typedef struct st_4107e0_1 {
    char padding_0[1144];
    struct st_4107e0_0 *field_478;
} st_4107e0_1;

unsigned int sub_4107e0(st_4107e0_1 *a0)
{
    st_4107e0_0 *idx;  // edi
    unsigned int v6;  // esi
    st_4107e0_0 *v0;  // [bp-0x14]
    st_4107e0_0 *v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    st_4107e0_1 *v3;  // [bp-0x4]

    v3 = a0;
    idx = a0->field_478;
    if (*((int *)&idx->padding_10[0]) >= 50)
        return 1;
    v2 = v6;
    if (*((int *)&idx->padding_10[0]) != idx->field_c)
    {
        v1 = idx;
        sub_40fbe0(&v3, 1);
        v0 = idx;
        sub_40fbe0(&edi, 1);
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
    idx->field_2b = idx->field_2b + 3;
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
        idx->field_20 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_20 = nan;
        /* unsupported instruction */
    }
    sub_464a80();
    return 0;
}
