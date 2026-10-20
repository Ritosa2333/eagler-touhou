// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e12d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

extern st_47dc29_1 g_4b42f8;

int sub_47e12d(void)
{
    int v2;  // edx
    unsigned int v0;  // [bp+0x4]
    unsigned int v1;  // [bp+0xc]

    sub_47dc29(&g_4b42f8.field_0, v2, v0, v1);
    return;
}
