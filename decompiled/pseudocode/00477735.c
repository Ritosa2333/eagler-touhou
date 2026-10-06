// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477735; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477735_0 {
    char padding_0[68];
    unsigned int field_44;
} st_477735_0;

unsigned int sub_477735(unsigned int a0, unsigned int a1)
{
    st_477735_0 *v1;  // esi
    unsigned int v2;  // eax
    int v0;  // [bp-0x4]

    v1 = sub_4753ee(v0);
    if (v1 && (v1->field_44 || !(v2 = sub_4777ce(36), v1->field_44 = v2, !v2)))
        return v1->field_44;
    *((unsigned int *)sub_46e96f()) = 12;
    return 0;
}
