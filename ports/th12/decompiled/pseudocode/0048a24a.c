// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a24a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48a24a(void)
{
    unsigned int v6;  // ecx
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp+0x4]
    unsigned int *v3;  // [bp+0x8]
    unsigned int v4;  // [bp+0xc]
    unsigned int v5;  // [bp+0x10]

    v1 = v5;
    v0 = v4;
    if (!v2)
    {
        sub_48ddbf(&v2);
        *(v3) = v2;
        return;
    }
    sub_48dc40(&v6);
    *(v3) = v6;
    v3[1] = v6;
    return;
}
