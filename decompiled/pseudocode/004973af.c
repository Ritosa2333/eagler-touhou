// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4973af; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4973af_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4973af_0;

int sub_4973af(void)
{
    st_4973af_0 *v1;  // ebp

    return sub_46cf1e(v1->field_4 + 21688, 1204, 4, sub_4026e0);
}
