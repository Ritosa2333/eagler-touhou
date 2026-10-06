// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4442b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4442b0(void)
{
    short *idx;  // edx
    unsigned int v2;  // eax
    unsigned int v3;  // ecx

    if (idx[11572 + v2] == v3)
        return;
    if (v2 && idx[11572] == v3)
        idx[11572] = idx[11572 + v2];
    if (v2 != 1 && idx[11573] == v3)
        idx[11573] = idx[11572 + v2];
    if (v2 != 2 && idx[11574] == v3)
        idx[11574] = idx[11572 + v2];
    if (v2 != 3 && idx[11575] == v3)
        idx[11575] = idx[11572 + v2];
    if (v2 != 4 && idx[11576] == v3)
        idx[11576] = idx[11572 + v2];
    idx[11572 + v2] = v3;
    sub_443920();
    sub_453d90(v3, 7);
    return;
}
