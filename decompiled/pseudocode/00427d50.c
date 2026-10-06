// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427d50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427d50_1 {
    char padding_0[4];
    struct st_427d50_0 *field_4;
} st_427d50_1;

typedef struct st_427d50_0 {
    char padding_0[8];
    struct st_427d50_1 *field_8;
} st_427d50_0;

int sub_427d50(void)
{
    st_427d50_1 *v1;  // ecx
    st_427d50_0 *v2;  // eax

    v1->field_4 = v2;
    v2->field_8 = v1;
    return;
}
