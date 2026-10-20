// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4931fc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

int sub_4931fc(void)
{
    unsigned int v2;  // fpround
    unsigned int v3;  // 4121
    unsigned int v0;  // [bp-0x4]

    if (!g_4d52dc)
        goto LABEL_0x493216;
    v3 = _ccall(v2);
    *((unsigned short *)&v0) = v3;
    if (((unsigned short)v0 & 127) == 127)
        goto LABEL_0x4931e9;
    else
        goto LABEL_0x493216;
}
