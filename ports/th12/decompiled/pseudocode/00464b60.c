// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464b60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

char sub_464b60(unsigned int i, char *iter)
{
    char v1;  // al

    v1 = 0;
    if (i)
    {
        do
        {
            v1 += *(iter);
            i -= 1;
            iter += 1;
        } while (i);
    }
    return v1;
}
