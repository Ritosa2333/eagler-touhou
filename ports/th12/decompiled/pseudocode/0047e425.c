// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e425; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e425_0 {
    char padding_0[4];
    unsigned int field_4;
    int field_8;
} st_47e425_0;


int sub_47e425(st_47e425_0 *a0, unsigned int a1, char *a2, unsigned int a3)
{
    return sub_47e144(a2, a3, a0->field_4, a0->field_8);
}
