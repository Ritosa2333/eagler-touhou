// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4094b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_4094b0(unsigned int *idx)
{
    int v2;  // ecx
    unsigned int *iter;  // eax
    int v4;  // ecx
    int v0;  // [bp-0x4]

    sub_4027e0(v0);
    idx[317] = idx[317] & 0xfffffffe;
    idx[322] = idx[322] & 0xfffffffe;
    v2 = 12;
    iter = idx + 452;
    do
    {
        v4 = v2;
        *(iter) = *(iter) & 0xfffffffe;
        iter += 13;
        v2 = v4 - 1;
    } while (v4 - 1 >= 0);
    idx[634] = idx[634] & 0xfffffffe;
    return idx;
}
