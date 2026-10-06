// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449d80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_449d80(void)
{
    char *v1;  // eax
    char v2;  // cl
    char *iter;  // eax
    unsigned int *v4;  // edx

    v2 = *(v1);
    if (*(v1) != 10)
    {
        do
        {
            if (v2 == 13)
                break;
            if (!*(v4))
                return;
            iter += 1;
            *(v4) = *(v4) - 1;
            v2 = *(iter);
        } while (*(iter) != 10);
    }
    if (!*(v4))
        return;
    while (1)
    {
        if (*(iter) != 10 && *(iter) != 13)
            return;
        if (!*(v4))
            return;
        iter += 1;
        *(v4) = *(v4) - 1;
    }
}
