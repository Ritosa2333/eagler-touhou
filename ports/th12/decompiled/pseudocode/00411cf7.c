// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411cf7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411cf7_1 {
    unsigned int field_0;
} st_411cf7_1;

typedef struct st_411cf7_0 {
    char padding_0[8];
    struct st_411cf7_1 *field_8;
} st_411cf7_0;

int sub_411cf7(void)
{
    unsigned int v1;  // edx
    unsigned int v2;  // esi
    st_411cf7_0 **v3;  // eax

    *(v3)->field_8(v1, v2);
    return;
}
