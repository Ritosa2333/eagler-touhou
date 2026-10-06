// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460f27; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460f27_0 {
    char padding_0[188];
    struct st_460f27_1 *field_bc;
} st_460f27_0;

typedef struct st_460f27_1 {
    unsigned int field_0;
} st_460f27_1;

extern unsigned int g_4cee38;

int sub_460f27(void)
{
    int v1;  // eax
    int v2;  // edx
    st_460f27_0 *v3;  // ecx

    v3->field_bc(v1, v2);
    g_4cee38 = 1;
    sub_4611d0(v1, v2);
    return;
}
