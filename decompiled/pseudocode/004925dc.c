// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4925dc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4925dc_0 {
    char padding_0[148];
    unsigned int field_94;
} st_4925dc_0;

typedef struct st_4925dc_1 {
    char padding_0[8];
    unsigned int field_8;
} st_4925dc_1;

void sub_4925dc(void)
{
    st_4925dc_0 *v1;  // eax
    st_4925dc_1 *v2;  // ebp

    v1 = sub_475467();
    v1->field_94 = v2->field_8;
    sub_46ff8f(0, NULL);
    __debugbreak();
}
