// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485b44; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_485b44(unsigned int a0, char *iter)
{
    unsigned int v1;  // esi
    char node;  // cl

    v1 = 0;
    while (1)
    {
        node = *(iter);
        if (!*(iter))
            break;
        iter += 1;
        if (node - 97 <= 5)
        {
            node -= 39;
        }
        else if (node - 65 <= 5)
        {
            node -= 7;
        }
        v1 = v1 * 16 + node - 48;
    }
    return v1;
}
