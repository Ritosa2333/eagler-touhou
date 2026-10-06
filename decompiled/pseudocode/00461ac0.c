// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461ac0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_461ac0(int a0)
{
    unsigned int *idx;  // eax
    unsigned int *index;  // esi

    idx = sub_461920(a0);
    if (idx)
    {
        idx[268] = *(index);
        idx[269] = index[1];
        idx[270] = index[2];
    }
    return idx;
}
