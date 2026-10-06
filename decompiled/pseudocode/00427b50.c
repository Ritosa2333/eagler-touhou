// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427b50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_427b50(void)
{
    unsigned int *v1;  // eax
    unsigned int *iter;  // eax
    unsigned int v3;  // edx
    unsigned int v4;  // ftop
    unsigned int v5;  // edx
    unsigned int v6;  // ftop
    unsigned int v7;  // ftop

    iter = v1 + 625;
    v3 = 2648;
    do
    {
        v5 = v3;
        if (*(iter) && *(iter) == 1)
        {
            v6 = v4 - 1;
            if (/* unsupported instruction */)
            {
                v7 = v6 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v7 = v6 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            *(iter) = 3;
            if (/* unsupported instruction */)
            {
                iter[3] = /* unsupported instruction */;
                /* unsupported instruction */
                v4 = v7 + 1;
            }
            else
            {
                iter[3] = nan;
                /* unsupported instruction */
                v4 = v7 + 1;
            }
        }
    } while ((iter += 2520, v3 = v5 - 1, v5 != 1));
    return;
}
