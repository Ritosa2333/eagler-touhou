// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f2c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f2c0_0 {
    char field_0;
} st_44f2c0_0;

extern char g_4b0ec8;
extern st_44f2c0_0 *g_4b2ec8;
extern char g_4b2ecc;

void sub_44f2c0(void)
{
    char *v1;  // eax
    char *v2;  // eax
    int v4;  // edi
    int v5;  // ebx

    if (g_4b2ec8 == &g_4b0ec8)
        return;
    sub_464220("---------------------------------------------------------- \r\n");
    if (g_4b2ecc)
        MessageBoxA(NULL, &g_4b0ec8, "log", MB_ICONSTOP);
    v1 = &g_4b0ec8;
    do
    {
        v2 = v1;
        v1 = v2 + 1;
    } while (*(v2));
    sub_463e80(&g_4b0ec8, v4, v5);
    return;
}
