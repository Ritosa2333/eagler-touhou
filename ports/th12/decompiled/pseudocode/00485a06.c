// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485a06; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_485a06_0 {
    unsigned int field_0;
} st_485a06_0;


int sub_485a06(unsigned int a0, int a1, st_485a06_0 **a2)
{
    int i;  // ebx
    int v3;  // eax
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    int v6;  // esi
    int *v7;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    i = 0;
    v3 = 1;
    if (a1 >= 0)
    {
        v1 = v4;
        v0 = v5;
        do
        {
            if (!v3)
                break;
            v6 = (a1 + i) / 2;
            v7 = a0 + v6 * 8;
            v3 = sub_46e3f8(*(a2), *(v7));
            if (!v3)
            {
                *(a2) = v7 + 1;
            }
            else if (v3 < 0)
            {
                a1 = v6 - 1;
            }
            else
            {
                i = v6 + 1;
            }
        } while (i <= a1);
    }
    return !v3;
}
