// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fc40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fc40_0 {
    char padding_0[292];
    unsigned int field_124;
} st_45fc40_0;

void sub_45fc40(int a0, unsigned int a1)
{
    int v2;  // edx
    st_45fc40_0 *v3;  // eax
    int v4;  // edi

    v3 = sub_45fc90(v2, sub_4622e0("::loadAnim : %s\n", a0));
    if (!v3)
        return;
    v3->field_124 = 1;
    if (!v3->field_124)
        return;
    do
    { } while (*((int *)(sub_45ffc0(v4) + 292)));
    return;
}
