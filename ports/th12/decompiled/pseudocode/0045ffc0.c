// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45ffc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45ffc0_1 {
    int field_0;
    char padding_4[260];
    struct st_45ffc0_0 *field_108;
    char padding_10c[24];
    int field_124;
} st_45ffc0_1;

typedef struct st_45ffc0_0 {
    char padding_0[6];
    unsigned short field_6;
} st_45ffc0_0;

unsigned int sub_45ffc0(void)
{
    st_45ffc0_1 *v7;  // edi
    int v8;  // esi
    int v9;  // ebx
    int v0;  // [bp-0x28], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x1c]
    int v2;  // [bp-0x14]
    int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    v0 = "::postloadAnim : %d, %d\n";
    sub_4622e0("::postloadAnim : %d, %d\n", v7->field_0, v7->field_124, v8, v9);
    v4 = 0;
    v2 = 0;
    v3 = 0;
    v5 = 0;
    if (v7->field_124 == 1)
    {
        v0 = 0;
        if (sub_4600a0(v7, v3, 0, v2, v7->field_108) < 0)
        {
            v7->field_124 = 0;
            return 0;
        }
        v1 = 1;
    }
    v0 += v7->field_108->field_6;
}
