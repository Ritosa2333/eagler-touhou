// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45400d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d4754;

int sub_45400d(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v0;  // ebx
    int i;  // eax
    unsigned int *v2;  // ebp
    unsigned int *j;  // edx
    unsigned int *iter;  // ebp
    unsigned int v5;  // ecx
    unsigned int *v6;  // ebp
    unsigned int *v7;  // edx

    if (g_4d4754 != v0)
        sub_4663e0();
    i = 0;
    j = v2 + 67;
    do
    {
        if (!*(iter))
            break;
        i += 1;
        v5 = 67;
        v6 = iter + 67;
        for (v7 = j + 67; v5; j += 1)
        {
            v5 -= 1;
            *(iter) = *(j);
            iter += 1;
        }
        v2 = v6;
        j = v7;
    } while (i < 31);
    if (a3)
        goto LABEL_0x453ff5;
}
