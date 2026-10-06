// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428560_0 {
    char padding_0[1152];
    unsigned int field_480;
} st_428560_0;

extern char g_4a0704;

int sub_428560(void)
{
    st_428560_0 *v3;  // eax
    st_428560_0 *idx;  // edi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    sub_427ec0(v0, v1);
    idx = &v3->padding_0[1108];
    *((char **)&v3->padding_0[0]) = &g_4a0704;
    sub_477420(idx, 0, 520);
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
        *((int *)&idx->padding_0[44]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx->padding_0[44]) = nan;
        /* unsupported instruction */
    }
    sub_4027e0();
    sub_4027e0();
    return;
}
