// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e442; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e442_0 {
    char padding_0[4];
    unsigned int field_4;
} st_47e442_0;

int sub_47e442(st_47e442_0 *a0, unsigned int a1, int a2, int a3)
{
    if (a0->field_4 != 1)
        return a2;
    return sub_47e144(a2, a3, " ?? ", 4);
}
