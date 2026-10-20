// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472b81; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_472b81_0 {
    char padding_0[124];
    struct st_472b81_1 *field_7c;
} st_472b81_0;

typedef struct st_472b81_1 {
    unsigned int field_0;
} st_472b81_1;

void sub_472b81(void)
{
    st_472b81_0 *v1;  // eax

    v1 = sub_475467();
    if (v1->field_7c)
        v1->field_7c();
    sub_472b48(); /* do not return */
}
