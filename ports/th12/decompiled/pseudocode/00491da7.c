// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491da7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_491da7(unsigned int *index)
{
    unsigned int *v2;  // eax
    unsigned int *iter;  // eax
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (index == *((int *)(sub_475467(v0, &v1) + 152)))
    {
        v2 = sub_475467();
        v2[38] = index[1];
        return v2;
    }
    iter = *((int *)(sub_475467() + 152));
    while (1)
    {
        if (!iter[1])
        {
            sub_472b94(); /* do not return */
        }
        else if (index != iter[1])
        {
            iter = iter[1];
        }
        else
        {
            iter[1] = index[1];
            return iter;
        }
    }
}
