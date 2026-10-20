// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4523e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4523e0_0 {
    char padding_0[96];
    unsigned int field_60;
} st_4523e0_0;

extern st_4523e0_0 *g_4b44e8;
extern unsigned int g_4ce55c;

unsigned int sub_4523e0(int *idx)
{
    unsigned int v3;  // edi
    unsigned int v0;  // [bp-0xc]
    int *v1;  // [bp-0x4], Other Possible Types: unsigned int

    v1 = idx;
    if (g_4ce55c)
        return 7;
    v0 = v3;
    v1 = idx[7];
    if (v1)
    {
        if (idx[13] < v1)
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
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            idx[6] = sub_4931e0();
        }
        else
        {
            idx[6] = 0xff;
        }
        if (idx[6] < 0)
            idx[6] = 0;
    }
    if (idx[13] >= v1 + 2)
        return 7;
    if (g_4b44e8 && (char)(g_4b44e8->field_60 >> 2 | g_4b44e8->field_60) & 1)
        return 1;
    sub_464a80();
    return 1;
}
