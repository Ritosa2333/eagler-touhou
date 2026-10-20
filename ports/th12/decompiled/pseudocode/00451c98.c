// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451c98; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451c98_0 {
    char padding_0[12];
    struct st_451c98_1 *field_c;
} st_451c98_0;

typedef struct st_451c98_1 {
    unsigned int field_0;
} st_451c98_1;

unsigned int sub_451c98(unsigned int a0, st_451c98_0 *a1, unsigned int a2, unsigned int a3)
{
    unsigned int v0;  // eax

    a1->field_c(a0, a3 + 4, v0);
    return 1;
}
