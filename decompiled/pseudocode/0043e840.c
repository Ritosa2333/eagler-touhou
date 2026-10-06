// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43e840(void)
{
    int v1;  // esi
    unsigned int v3;  // esi

    v3 = (!sub_46c9ea(740, v1) ? 0 : sub_43e3e0());
    if (!sub_43e470(v3))
        return v3;
    if (!v3)
        return 0;
    sub_43e520(v3);
    sub_46ca4f(v3);
    return 0;
}
