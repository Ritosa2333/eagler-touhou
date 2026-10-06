// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ff67; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b3d5c;

unsigned int sub_46ff67(unsigned int a0)
{
    unsigned int *v1;  // eax
    char v0;  // [bp+0x0]

    v1 = sub_4751de(g_4b3d5c, &v0);
    if (!v1)
    {
        return 0;
    }
    else if (v1(a0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
