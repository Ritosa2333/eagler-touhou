// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_425900(int *idx)
{
    int v1;  // eax

    if (idx[587])
        sub_46c941(idx[587]);
    idx[587] = 0;
    v1 = idx[286];
    if (v1)
        v1 = sub_46c941(v1);
    idx[286] = 0;
    return v1;
}
