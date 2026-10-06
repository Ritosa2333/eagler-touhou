// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48f735; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

int sub_48f735(void)
{
    unsigned int v4;  // ecx
    unsigned int v5;  // fpround
    unsigned int v6;  // 4120
    unsigned int v7;  // sseround
    unsigned int v8;  // 4118
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    int v2;  // [bp+0x4]
    unsigned int v3;  // [bp+0x8]

    v1 = v4;
    v0 = v4;
    if (v2 == 589855 && v3 == 0xffffffff)
    {
        v6 = _ccall(v5);
        *((unsigned short *)&v1) = v6;
        if ((unsigned short)(v1 & 7997) == 573)
        {
            if (g_4d52dc)
            {
                v8 = _ccall(v7 & 3);
                v0 = v8;
                if ((v0 & 65216) == 7808)
                    return;
            }
            else
            {
                return;
            }
        }
    }
    if (sub_48e1cf(0, v2, v3 & 0xfff7ffff))
        sub_470dc6(0, 0, 0, 0, 0);
    return;
}
