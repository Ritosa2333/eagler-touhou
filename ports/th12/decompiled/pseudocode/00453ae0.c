// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453ae0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void* g_4ce940;
extern char g_4ceacb;
extern char g_4ceae8;
extern unsigned int g_4cf4e8;
extern unsigned int g_4cf4f8;
extern unsigned int g_4cf4fc;
extern unsigned int g_4cf500;
extern unsigned int g_4d0da8[4];
extern unsigned int g_4d4758;

unsigned int sub_453ae0(void)
{
    int v6;  // edi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]
    int v2;  // [bp+0x0]
    int v3;  // [bp+0x4]
    int v4;  // [bp+0x8]
    int v5;  // [bp+0xc]

    if (!g_4cf4f8)
    {
        return 0xffffffff;
    }
    else if (!g_4ceacb)
    {
        return 0xffffffff;
    }
    else if (!g_4cf4e8)
    {
        return 0xffffffff;
    }
    else if (!(g_4ceae8 & 16))
    {
        return sub_453890();
    }
    else if (!g_4d0da8[v6])
    {
        return 0xffffffff;
    }
    else
    {
        sub_454b00("Streming BGM Load no %d\r\n", v6);
        g_4d4758 = CreateEventA(NULL, 0, 0, NULL);
        g_4cf500 = CreateThread(NULL, NULL, sub_4548a0, g_4ce940, THREAD_CREATE_RUN_IMMEDIATELY, &g_4cf4fc);
        return sub_453b8e(v0, v1, v2, v3, v4, v5);
    }
}
