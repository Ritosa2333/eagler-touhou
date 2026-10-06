// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4529a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_4529a0(int a0, int a1, int a2, int a3, int a4, int a5)
{
    int v0;  // esi
    unsigned int *idx;  // esi

    idx = sub_46c9ea(0x44, v0);
    if (!idx)
        return NULL;
    idx[16] = idx[16] & 0xfffffffe;
    sub_477420(idx, 0, 0x44);
    *(idx) = *(idx) | 2;
    sub_452a60(idx, a0, a1, a2, a3, a4, a5);
    return idx;
}
