// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410270; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_410270(void)
{
    int idx;  // eax
    unsigned int *index;  // edx
    unsigned int v4;  // edi
    unsigned int v5;  // eax
    unsigned int v0;  // [bp-0x8]

    idx = 0;
    if (*(index) - 1 <= 0)
        return;
    v0 = v4;
    do
    {
        *((unsigned int *)(*((int *)(index[3] + idx * 4)) + 1148)) = *((int *)(*((int *)(index[3] + idx * 4)) + 1148)) & 0xffffff1f | (v5 & 7) * 32;
        idx += 1;
    } while (idx < *(index) - 1);
    return;
}
