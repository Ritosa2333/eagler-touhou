// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422be0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422be0_0 {
    char padding_0[96];
    char field_60;
} st_422be0_0;

extern unsigned int g_4ce8cc;

unsigned int sub_422be0(st_422be0_0 *a0)
{
    if (!(a0->field_60 & 4))
        memset(g_4ce8cc + 160, 0, 16);
    return 1;
}
