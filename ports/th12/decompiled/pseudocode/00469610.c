// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469610; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_469610(void)
{
    void* idx;  // eax
    char v2;  // dl
    unsigned int *v0;  // [bp+0x4]

    if ((int)idx[0x1000] + 4 >= 0x1000)
        return;
    if (v2)
    {
        *(idx[0x1000] + (char *)idx) = v2;
        *((int *)&idx[0x1000]) = (int)idx[0x1000] + 4;
    }
    *((unsigned int *)(idx[0x1000] + (char *)idx)) = *(v0);
    *((int *)&idx[0x1000]) = (int)idx[0x1000] + 4;
    return;
}
