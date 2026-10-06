// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411cd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411cd0_0 {
    char padding_0[8];
    char field_8;
    char padding_9[7];
    int field_10;
} st_411cd0_0;

typedef struct st_411cd0_1 {
    char padding_0[4];
    struct st_411cd0_0 *field_4;
} st_411cd0_1;

int sub_411cd0(void)
{
    st_411cd0_1 *v1;  // eax

    if (!(*((char *)(*((int *)&v1->field_4->padding_0[4]) + 8)) & 1))
        return;
    if (*((int *)(*((int *)&v1->field_4->padding_0[4]) + 16)) >= 0)
        return;
    sub_411cf7();
    return;
}
