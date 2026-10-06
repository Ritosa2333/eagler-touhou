// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dda9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dda9_1 {
    struct st_47dda9_0 *field_0;
} st_47dda9_1;

typedef struct st_47dda9_0 {
    char padding_0[8];
    unsigned int field_8;
} st_47dda9_0;


int sub_47dda9(st_47dda9_1 **a0, unsigned int a1, unsigned int a2)
{
    if (!*(a0))
        return a2;
    goto *((void *)(*(a0)->field_0->field_8));
}
