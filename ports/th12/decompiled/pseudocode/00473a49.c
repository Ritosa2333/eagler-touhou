// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473a49; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473a49_0 {
    char padding_0[4];
    unsigned int field_4;
} st_473a49_0;

typedef struct st_473a49_1 {
    char padding_0[112];
    unsigned int field_70;
} st_473a49_1;

extern unsigned int g_4b40c0;

unsigned int sub_473a49(void)
{
    int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // eax
    st_473a49_0 *v0;  // [bp-0x14]
    st_473a49_1 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]

    sub_46d3b4(0, v4);
    g_4b40c0 = 0;
    if (v5 == 0xfffffffe)
    {
        g_4b40c0 = 1;
        v6 = GetOEMCP();
    }
    else if (v5 == 0xfffffffd)
    {
        g_4b40c0 = 1;
        v6 = GetACP();
    }
    else if (v5 == 0xfffffffc)
    {
        v6 = v0->field_4;
        g_4b40c0 = 1;
    }
    else
    {
        if (!v2)
            return v5;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return v5;
    }
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v6;
}
