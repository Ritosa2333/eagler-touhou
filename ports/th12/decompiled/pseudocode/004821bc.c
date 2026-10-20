// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4821bc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aaed8;
extern unsigned int g_4b42f8;
extern unsigned int g_4b42fc;
extern unsigned int g_4b4300;
extern unsigned int g_4b4304;
extern unsigned int g_4b4308;

int sub_4821bc(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    void* idx;  // ebp
    unsigned int v1;  // edi

    sub_46fcec(&g_4aaed8, 100);
    v1 = (int)idx[20];
    if (!v1 || !sub_46ebd7(5))
        return sub_46fd31();
    sub_46ec9a(5);
    *((unsigned int *)((char *)idx - 4)) = 0;
    g_4b42f8 = v1;
    g_4b42fc = (int)idx[24];
    g_4b4308 = 0;
    g_4b4300 = 0;
    g_4b4304 = 0;
    sub_47e053((int)idx[8], (int)idx[12], (int)idx[16], 0, (short)idx[28]);
    *((unsigned int *)((char *)idx - 28)) = sub_481f3b((int)idx[8], (int)idx[12], (int)idx[16], 0, (short)idx[28]);
    sub_47d5c0();
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_48224d();
    return sub_46fd31();
}
