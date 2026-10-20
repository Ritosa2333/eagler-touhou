// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d100; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int * sub_43d100(void)
{
    int *v1;  // esi

    if (*(v1))
    {
        sub_46c941(*(v1));
        *(v1) = 0;
    }
    if (v1[1])
    {
        sub_46c941(v1[1]);
        v1[1] = 0;
    }
    sub_46ca4f(v1);
    return v1;
}
