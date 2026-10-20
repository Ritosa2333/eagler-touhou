// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f130; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b50c4;
extern unsigned int g_4ce8cc;

unsigned int sub_43f130(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    v3 = &(&g_4b50c4)[g_4ce8cc];
    if (!*((int *)v3))
        return v5;
    sub_4604e0(v0, v1);
    v4 = sub_46ca4f(*((int *)v3));
    *((unsigned int *)v3) = 0;
    return v4;
}
