// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43aac0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43aac0_0 {
    char padding_0[28];
    char field_1c;
} st_43aac0_0;

typedef struct st_43aac0_1 {
    char padding_0[48];
    unsigned int field_30;
    char padding_34[64];
    struct st_43aac0_0 *field_74;
} st_43aac0_1;

extern unsigned int g_4ce8cc;

unsigned int sub_43aac0(unsigned int a0, st_43aac0_1 *idx)
{
    unsigned int v4;  // esi
    unsigned int v5;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    sub_461920(*((int *)(idx->field_74->field_1c * 228 + a0 + 33324)), g_4ce8cc, *((int *)(idx->field_74->field_1c * 228 + a0 + 33324)));
    v1 = v5;
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
    sub_4646e0();
    v0 = v5;
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
    sub_4646e0();
    if (!/* unsupported instruction */)
    {
        idx->field_30 = nan;
        /* unsupported instruction */
        return 0;
    }
    idx->field_30 = /* unsupported instruction */;
    /* unsupported instruction */
    return 0;
}
