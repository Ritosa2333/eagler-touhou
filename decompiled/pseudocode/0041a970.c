// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a970_3 {
    char padding_0[5964];
    struct st_41a970_1 *field_174c;
} st_41a970_3;

typedef struct st_41a970_1 {
    char padding_0[4];
    struct st_41a970_0 *field_4;
} st_41a970_1;

typedef struct st_41a970_0 {
    char padding_0[8];
    unsigned short field_8;
} st_41a970_0;

int sub_41a970(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    st_41a970_3 *v5;  // ecx
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]
    unsigned int v2;  // [bp+0x4]

    v1 = v3;
    v0 = v4;
    if (*((short *)(*((int *)&v5->field_174c->field_4->padding_0[4]) + 8)) & 1 << ((char)v2 & 31))
    {
        v2 = v6;
        sub_468f70();
    }
    return;
}
