// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e1ce; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_47e1ce(void* idx, unsigned int a1, unsigned int a2)
{
    char v0;  // al
    unsigned int *v1;  // eax

    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    if (a2 != 2 && a2 != 3)
        v0 = 0;
    else
        v0 = a2;
    *((unsigned int **)idx) = NULL;
    *((char *)&idx[4]) = v0;
    if (a2 != 1)
        return idx;
    v1 = sub_47deeb(1);
    *((unsigned int **)idx) = v1;
    if (v1)
        return idx;
    *((char *)&idx[4]) = 3;
    return idx;
}
