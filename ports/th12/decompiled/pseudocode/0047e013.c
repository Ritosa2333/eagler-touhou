// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e013; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47e013(void)
{
    unsigned int v1;  // edx
    unsigned int v2;  // esi
    char *v3;  // eax
    char *iter;  // eax
    unsigned int i;  // esi
    unsigned int v6;  // ecx

    v2 = v1;
    if (!v2)
        return;
    iter = v3;
    do
    {
        i = v2;
        *(iter) = *((char *)(v6 - v3 + iter));
        iter += 1;
        v2 = i - 1;
    } while (i != 1);
    return;
}
