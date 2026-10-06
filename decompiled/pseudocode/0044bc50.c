// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bc50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44bc50(int *a0)
{
    int v1;  // eax
    int v2;  // eax

    v1 = *(a0);
    if (!v1)
        return v1;
    v2 = sub_46c941(v1);
    *(a0) = 0;
    return v2;
}
