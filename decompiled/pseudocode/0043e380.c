// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e380; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_43e380(void)
{
    int *idx;  // esi
    unsigned int v3;  // edi
    int index;  // edi
    unsigned int v0;  // [bp-0x4]

    if (!idx[13])
        return;
    v0 = v3;
    index = 0;
    if (idx[14] > 0)
    {
        do
        {
            if (*((int *)(idx[13] + index * 4)))
            {
                sub_46c941(*((int *)(idx[13] + index * 4)));
                *((unsigned int *)(idx[13] + index * 4)) = 0;
            }
        } while ((index = (int)(index + 1), index < idx[14]));
    }
    if (idx[13])
    {
        sub_46c941(idx[13]);
        idx[13] = 0;
    }
    idx[13] = 0;
    return;
}
