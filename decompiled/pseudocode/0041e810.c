// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41e810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41e810_1 {
    struct st_41e810_0 *field_0;
    unsigned int field_4;
} st_41e810_1;

typedef struct st_41e810_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_41e810_0;

int sub_41e810(void)
{
    st_41e810_1 *v1;  // eax
    unsigned int v2;  // edx
    unsigned int i;  // esi

    do
    {
        v1->field_0->field_47c = v1->field_0->field_47c | v2;
    } while (v1->field_4 != i);
    sub_41e810();
    return;
}
