// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406840_0 {
    char padding_0[40];
    int field_28;
} st_406840_0;

int sub_406840(st_406840_0 *a0)
{
    return a0->field_28 >= 60;
}
