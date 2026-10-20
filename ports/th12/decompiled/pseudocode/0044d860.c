// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d860; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d860_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_44d860_0;

int sub_44d860(void)
{
    st_44d860_0 *v2;  // ecx
    unsigned int v3;  // esi
    unsigned int *idx;  // eax
    unsigned int v5;  // ecx
    unsigned int *v6;  // edx
    unsigned int v0;  // [bp-0x4]

    if (v2->field_3)
    {
        v0 = v3;
        *(idx) = *(idx) + v2->field_2;
        v5 = v2->field_0;
        idx[1] = idx[1] + v2->field_1;
        idx[2] = idx[2] + v5;
        *(v6) = *(v6) + 1;
    }
    return;
}
