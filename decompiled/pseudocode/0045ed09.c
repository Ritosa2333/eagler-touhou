// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45ed09; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45ed09_0 {
    char padding_0[400];
    struct st_45ed09_1 *field_190;
} st_45ed09_0;

typedef struct st_45ed09_1 {
    unsigned int field_0;
} st_45ed09_1;

int sub_45ed09(void)
{
    unsigned int v1;  // eax
    st_45ed09_0 *v2;  // ecx

    v2->field_190(v1);
    return;
}
