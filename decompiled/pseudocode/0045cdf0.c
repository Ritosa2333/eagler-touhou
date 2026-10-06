// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45cdf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45aa30_0 {
    char padding_0[60];
    unsigned int field_3c;
    char padding_40[1084];
    unsigned int field_47c;
} st_45aa30_0;


int sub_45cdf0(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    st_45aa30_0 *v5;  // eax
    int v6;  // ecx
    int v7;  // ecx
    unsigned int v8;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v3;
    v0 = v4;
    switch (v5->field_47c >> 23 & 31)
    {
    case 0:
        sub_45a570(v6, v6 + 12);
        return;
    case 1:
        sub_45af10(v7);
        return;
    case 2: case 3:
        sub_45aa30(v8, v8 + 36, v5);
        return;
    default:
        return;
    }
}
