// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d3e9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ab168;

void sub_48d3e9(unsigned int a0)
{
    int v1;  // esi
    void* v2;  // ebp
    int v3;  // edi
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4ab168, 16);
    v1 = sub_47c025(&g_4ab168, 16) + 32;
    *((int *)((char *)v2 - 28)) = v1;
    if (!(int)v2[12])
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        sub_47c0fc(v1);
        *((unsigned int *)((char *)v2 - 4)) = 0;
        v3 = sub_48d319(v1);
        *((unsigned int *)((char *)v2 - 32)) = (int)v2[8](v1, (int)v2[12], (int)v2[16], (int)v2[20]);
        sub_48d3b5(v3, v1);
        *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
        sub_48d471();
    }
    sub_46fd31(&g_4ab168, 16, v0, a0);
    return;
}
