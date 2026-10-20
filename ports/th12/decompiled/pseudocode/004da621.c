// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da621; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4da621_0 {
    char padding_0[67];
    char field_43;
} st_4da621_0;

typedef struct st_4da621_1 {
    char padding_0[99];
    unsigned int field_63;
} st_4da621_1;

extern unsigned int g_188a0da6;

int sub_4da621(void)
{
    unsigned int v4;  // ecx
    st_4da621_0 *idx;  // eax
    st_4da621_0 *v6;  // ebp
    unsigned int v7;  // edx
    st_4da621_1 *v8;  // esi
    unsigned int v0;  // [bp-0xc]
    st_4da621_0 *v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = idx;
    g_188a0da6 = idx;
    if (v6 <= idx)
        return;
    v0 = v7 | *((int *)&(v8->padding_0)[1]);
    if ((v7 | *((int *)&(v8->padding_0)[1])) >= 0)
    {
        idx->field_43 = idx->field_43 ^ 244;
        __debugbreak();
    }
}
