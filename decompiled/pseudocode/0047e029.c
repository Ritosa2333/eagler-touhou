// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e029; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47e029(void)
{
    char *v1;  // ecx
    unsigned int v2;  // edx
    unsigned int v0;  // [bp+0x4]

    if (!v0)
        return;
    while (1)
    {
        v0 -= 1;
        if (v0 == 1)
        {
            return;
        }
        else if (!*(v1))
        {
            return;
        }
        else if (*(v1) == *((char *)v2))
        {
            v1 += 1;
            v2 += 1;
        }
        else
        {
            return;
        }
    }
}
