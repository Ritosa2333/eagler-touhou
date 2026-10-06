// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e41a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48e41a(char *a0, unsigned int a1)
{
    unsigned int v0;  // eax
    unsigned int v1;  // eax

    v0 = 0;
    if (a1 <= 0)
        return 0;
    do
    {
    } while (*(a0) && (v1 = v0 + 1, a0 += 1, v0 = v1, v0 < a1));
    return v1;
}
