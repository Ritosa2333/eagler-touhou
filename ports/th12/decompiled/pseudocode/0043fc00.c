// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43fc00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43fc00_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_43fc00_0;


int sub_43fc00(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // edi
    st_43fc00_0 *v5;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v3;
    v0 = v4;
    switch (v5->field_1c)
    {
    case 8:
        sub_4463c0(v5);
    case 10:
        sub_447e10(v5);
        return;
    case 11:
        sub_446c20();
        return;
    case 14:
        sub_448690(v5);
        return;
    case 15:
        sub_448fb0(v5);
        return;
    default:
        return;
    }
}
