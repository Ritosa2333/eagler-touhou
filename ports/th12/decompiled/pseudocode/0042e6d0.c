// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42e6d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42e6d0_0 {
    char padding_0[1106];
    unsigned short field_452;
} st_42e6d0_0;

typedef struct st_42e6d0_1 {
    char padding_0[1200];
    struct st_42e6d0_0 *field_4b0;
} st_42e6d0_1;

int sub_42e6d0(st_42e6d0_1 *a0)
{
    return a0->field_4b0->field_452 + 480;
}
