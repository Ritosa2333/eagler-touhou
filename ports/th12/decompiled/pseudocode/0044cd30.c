// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cd30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44cd30(unsigned int *idx, unsigned int a1, unsigned int a2)
{
    unsigned int v2;  // eax
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    idx[3] = sub_44c230(a2);
    idx[1] = sub_44c310(v0, v1);
    v2 = idx[3];
    idx[2] = v2;
    return _INSERT(v2, 0, v2);
}
