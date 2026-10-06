// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c370; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c370_0 {
    char padding_0[16];
    unsigned int field_10;
} st_45c370_0;

typedef struct st_45c370_1 {
    char padding_0[956];
    unsigned int field_3bc;
} st_45c370_1;

int sub_45c370(void)
{
    int v2;  // ecx
    st_45c370_0 *v3;  // eax
    st_45c370_0 *iter;  // eax
    unsigned int v5;  // esi
    int v6;  // ecx
    int i;  // ecx
    st_45c370_1 *v8;  // edx
    unsigned int v0;  // [bp-0x4]

    if (v2 <= 0)
        return;
    iter = &v3->field_10;
    v0 = v5;
    do
    {
        i = v6;
        *((unsigned int *)&iter->padding_0[0]) = v8->field_3bc;
        iter = &iter[1].padding_0[8];
        v6 = i - 1;
    } while (i != 1);
    return;
}
