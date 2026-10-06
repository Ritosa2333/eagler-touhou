// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439a40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_439a40_0 {
    char padding_0[2592];
    int field_a20;
    char padding_a24[4];
    unsigned int field_a28;
    char padding_a2c[32596];
    unsigned int field_8980;
    char field_8984;
    char padding_8985[15003];
    int field_c420;
} st_439a40_0;

extern char g_4d49d0;

unsigned int sub_439a40(unsigned int a0, unsigned int a1, st_439a40_0 *idx)
{
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v3 = a0;
    v2 = v4;
    v1 = v5;
    if (idx->field_a28 != 1)
    {
        idx->field_8980 = 0;
        idx->field_8984 = 0;
        return 0;
    }
    if (*((int *)&idx[1].padding_0[0]) < 0)
    {
        if (idx->field_a20 >= 99)
        {
            return 0;
        }
        else if (g_4d49d0 & 1)
        {
            sub_4067e0(0);
        }
        else
        {
            return 0;
        }
    }
    if (*((int *)&idx[1].padding_0[0]) != idx->field_c420)
        sub_4399d0();
    if (*((int *)&idx[1].padding_0[0]) < 14)
    {
        sub_464a80();
        return 0;
    }
    else if (g_4d49d0 & 1)
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
        v0 = a0;
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
        return 0;
    }
    else
    {
        sub_4067e0(-0x1);
        return 0;
    }
}
