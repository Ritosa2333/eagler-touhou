// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490d21; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_490d21(void)
{
    int *v2;  // eax
    int *i;  // edi
    int v4;  // eax
    int *v5;  // ecx
    int v6;  // esi
    unsigned int *iter;  // esi
    unsigned int *v0;  // [bp-0x8]

    i = v2;
    v4 = 0;
    if (!i)
        return;
    v5 = i;
    if (*(v5))
    {
        do
        {
            v5 += 1;
            v4 += 1;
        } while (*(v5));
    }
    v0 = sub_477813(v4 + 1, 4, v6);
    sub_472c0d(9);
    for (; *(i); i += 1)
    {
        *(iter) = sub_49132c(*(i));
        iter += 1;
    }
    *(iter) = 0;
    return;
}
