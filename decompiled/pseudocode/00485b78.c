// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485b78; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_485b78(unsigned int a0, char *a1)
{
    unsigned int v1;  // eax
    char *v2;  // edx

    v1 = 0;
    while (1)
    {
        v2 = a1 + 1;
        if ((*(a1) < 65 || *(a1) > 90) && *(a1) - 97 > 25)
            break;
        v1 += 1;
        a1 = v2;
    }
    return v1;
}
