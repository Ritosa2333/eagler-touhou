// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e2f7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void* sub_47e2f7(void* idx, unsigned int a1, unsigned int a2)
{
    unsigned int v0;  // eax

    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    *((char *)&idx[4]) = a2;
    if (a2 != 1)
    {
        *((unsigned int *)idx) = 0;
        return idx;
    }
    v0 = sub_47deeb(1);
    *((unsigned int *)idx) = v0;
    if (v0)
        return idx;
    *((char *)&idx[4]) = 3;
    return idx;
}
