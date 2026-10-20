// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483ee6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_483ee6(char *iter)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    char v5;  // al
    char *v6;  // eax
    char *v7;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (!*(iter))
        return;
    v1 = v3;
    v0 = v4;
    do
    {
        v5 = *(iter);
        if (*(iter) >= 48 && v5 <= 57)
        {
            *(iter) = v5 - 48;
            iter += 1;
        }
        else if (v5 != 59)
        {
            iter += 1;
        }
        else
        {
            v6 = iter;
            do
            {
                v7 = v6 + 1;
                *(v6) = *(v7);
                v6 = v7;
            } while (*(v6));
        }
    } while (*(iter));
    return;
}
