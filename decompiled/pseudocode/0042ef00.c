// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ef00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42ef00_0 {
    char padding_0[1252];
    struct st_42ef00_1 *field_4e4;
    int field_4e8;
    unsigned int field_4ec;
    unsigned int field_4f0;
    unsigned int field_4f4;
} st_42ef00_0;

typedef struct st_42ef00_1 {
    char field_0;
} st_42ef00_1;

unsigned int sub_42ef00(unsigned int a0)
{
    unsigned int v5;  // edi
    st_42ef00_0 *idx;  // esi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v3 = a0;
    v2 = v5;
    if (idx->field_4ec == 1)
    {
        sub_4615a0(idx->field_4e8, &v3, 0, 0);
        idx->field_4ec = idx->field_4ec + 1;
        idx->field_4e4 = &v3;
    }
    if (idx->field_4f0 == 1)
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
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
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = nan;
            /* unsupported instruction */
        }
        sub_411b80(v0, v1);
        idx->field_4f0 = idx->field_4f0 + 1;
    }
    idx->field_4f4 = idx->field_4f4 + 1;
    return 1;
}
