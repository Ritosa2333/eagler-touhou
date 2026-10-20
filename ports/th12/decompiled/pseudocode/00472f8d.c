// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472f8d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad134;
extern unsigned int g_4ad3f8[4];
extern unsigned int g_4ad3fc[4];
extern char g_4b3d86;
extern int g_4b3da8;
extern char g_4b3dc1;
extern char g_4b3ec5;
extern char g_4b40bc;

int sub_472f8d(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // edi
    unsigned int v5;  // edi
    int v6;  // eax
    HANDLE v7;  // ebx
    void* v8;  // esi
    unsigned int count;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x8]
    unsigned int *v2;  // [bp+0x4]

    v1 = v3;
    v0 = v4;
    v5 = 0;
    v1 = 0;
    do
    {
    } while (v2 != g_4ad3f8[2 * v5] && (v5 += 1, v1 = v5, v5 < 23));
    if (v5 >= 23)
        return;
    if (sub_4831b2(3) != 1 && (sub_4831b2(3) || g_4ad134 != 1))
    {
        if (v2 != 0xfc)
        {
            if (sub_47a2b9(&g_4b3da8, 788, "Runtime Error!\n\nProgram: "))
                sub_470dc6(0, 0, 0, 0, 0);
            g_4b3ec5 = 0;
            if (!GetModuleFileNameA(NULL, &g_4b3dc1, 260) && sub_47a2b9(&g_4b3dc1, 763, "<program name unknown>"))
                sub_470dc6(0, 0, 0, 0, 0);
            if (sub_475860(&g_4b3dc1) + 1 > 60 && !(v6 = (int)(sub_475860(&g_4b3dc1) + &g_4b3d86), !sub_4830fd(v6, &g_4b40bc - v6, "...", 3)))
                sub_470dc6(0, 0, 0, 0, 0);
            if (sub_483089(&g_4b3da8, 788, "\n\n"))
                sub_470dc6(0, 0, 0, 0, 0);
            if (sub_483089(&g_4b3da8, 788, g_4ad3fc[2 * v1]))
                sub_470dc6(0, 0, 0, 0, 0);
            sub_482f20(&g_4b3da8, "Microsoft Visual C++ Runtime Library", 73744);
            return;
        }
        else
        {
            return;
        }
    }
    v7 = GetStdHandle(STD_ERROR_HANDLE);
    if (!v7)
        return;
    if (v7 == 0xffffffff)
        return;
    v8 = &g_4ad3fc[2 * v5];
    count = sub_475860(*((int *)v8), &v3, 0);
    WriteFile(v7, *((int *)v8), count, &v3, NULL);
    return;
}
