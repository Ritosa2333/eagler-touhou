// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d2f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44d2f0(char *a0)
{
    char *v0;  // eax
    char *v1;  // eax

    v0 = a0;
    do
    {
        v1 = v0;
        v0 = v1 + 1;
    } while (*(v1));
    return v1 + 1 - (a0 + 1);
}
