// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46566c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46566c_0 {
    char padding_0[24];
    struct st_46566c_1 *field_18;
} st_46566c_0;

typedef struct st_46566c_1 {
    unsigned int field_0;
} st_46566c_1;

int sub_46566c(st_46566c_0 *a0, unsigned int a1)
{
    unsigned int v1;  // eax
    int v2;  // esi

    if (a0->field_18(v1, a1) < 0)
        goto LABEL_0x46567f;
    sub_465690(v2);
}
