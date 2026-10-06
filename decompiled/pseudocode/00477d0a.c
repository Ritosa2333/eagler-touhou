// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477d0a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477d0a_0 {
    char padding_0[188];
    unsigned int field_bc;
} st_477d0a_0;

int sub_477d0a(st_477d0a_0 **a0)
{
    return *(a0)->field_bc;
}
