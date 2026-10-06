// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x482256; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47d5c0_1 {
    unsigned int field_0;
} st_47d5c0_1;

typedef struct st_47d5c0_0 {
    char padding_0[4];
    struct st_47d5c0_1 *field_4;
    struct st_47d5c0_1 *field_8;
    struct st_47d5c0_1 *field_c;
} st_47d5c0_0;

extern int g_4aaef8;
extern st_47d5c0_0 g_4b42f8;
extern unsigned int g_4b42fc;
extern unsigned int g_4b4300;
extern unsigned int g_4b4304;
extern unsigned int g_4b4308;

int sub_482256(void)
{
    void* idx;  // ebp
    unsigned int v2;  // edi

    sub_46fcec(&g_4aaef8, 100);
    v2 = (int)idx[20];
    if (!v2 || !sub_46ebd7(5))
        return sub_46fd31();
    sub_46ec9a(5);
    *((unsigned int *)((char *)idx - 4)) = 0;
    *((unsigned int *)&g_4b42f8.padding_0[0]) = v2;
    g_4b42fc = (int)idx[24];
    g_4b4308 = 0;
    g_4b4300 = 0;
    g_4b4304 = 0;
    sub_47e053((int)idx[8], (int)idx[12], (int)idx[16], (int)idx[28], (int)idx[32]);
    *((unsigned int *)((char *)idx - 28)) = sub_481f3b((int)idx[8], (int)idx[12], (int)idx[16], (int)idx[28], (int)idx[32]);
    sub_47d5c0(&g_4b42f8.padding_0[0]);
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_4822e7();
    return sub_46fd31();
}
