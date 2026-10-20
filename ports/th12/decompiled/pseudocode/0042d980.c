// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42d980; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42d980_0 {
    char padding_0[760];
    int field_2f8;
    char padding_2fc[308];
    unsigned int field_430;
} st_42d980_0;

unsigned int sub_42d980(st_42d980_0 *idx)
{
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    st_42d980_0 *v0;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

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
    v2 = v4;
    v1 = v5;
    v0 = idx;
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
    sub_464a20();
    if (idx->field_2f8 > 0)
        return 0;
    idx->field_430 = idx->field_430 ^ 0x400;
    return 1;
}
