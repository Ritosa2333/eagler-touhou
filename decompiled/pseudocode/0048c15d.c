// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c15d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c16f_0 {
    char padding_0[12];
    unsigned int field_c;
} st_47c16f_0;

typedef struct st_48c15d_0 {
    char padding_0[12];
    struct st_47c16f_0 *field_c;
} st_48c15d_0;

void sub_48c15d(void)
{
    st_48c15d_0 *v1;  // ebp

    sub_47c16f(v1->field_c);
    return;
}
