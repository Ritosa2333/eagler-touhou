// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45edd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45edd0_0 {
    unsigned short field_0;
    char field_1;
} st_45edd0_0;

int sub_45edd0(void)
{
    st_45edd0_0 *v2;  // eax
    unsigned int v3;  // eax
    unsigned int v4;  // esi
    unsigned int *idx;  // ecx
    unsigned int *v6;  // edx
    unsigned int v0;  // [bp-0x4]

    if (*((char *)&v2->field_0 + 1))
    {
        v3 = v2->field_0;
        v0 = v4;
        *(idx) = *(idx) + (v3 >> 5 & 7);
        idx[1] = idx[1] + (v3 >> 2 & 7);
        idx[2] = idx[2] + (v3 & 3);
        *(v6) = *(v6) + 1;
    }
    return;
}
