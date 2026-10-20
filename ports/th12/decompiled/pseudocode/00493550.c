// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52d4;

int sub_493550(void)
{
    unsigned int v3;  // sseround
    unsigned int v4;  // 4121
    unsigned int v5;  // eax
    unsigned int v6;  // cc_op
    unsigned int v7;  // cc_dep2
    unsigned int v8;  // fpround
    unsigned int v9;  // 4106
    unsigned int v10;  // 4104
    unsigned short v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (g_4d52d4)
    {
        v4 = _ccall(v3 & 3);
        v1 = v4;
        v5 = v1 & 8064;
        v6 = 6;
        v7 = 8064;
        if (v5 == 8064)
        {
            v9 = _ccall(v8);
            v0 = v9;
            v6 = 5;
            v5 = v0 & 127;
            v7 = 127;
        }
        v10 = _ccall(4, v6, v5, v7, 0);
        if (v10 & 1)
            return sub_49580e();
    }
    sub_4956a5();
}
