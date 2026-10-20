// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453890; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453670_2 {
    char field_0;
} st_453670_2;

typedef struct st_453670_1 {
    char padding_0[6532];
    struct st_453670_2 *field_1984;
} st_453670_1;

extern st_453670_1 g_4cf4e8;
extern unsigned int g_4d4754;

unsigned int sub_453890(char *a0, unsigned int a1)
{
    unsigned int v2;  // esi
    int v3;  // eax
    unsigned int v0;  // [bp-0x4]

    if (g_4d4754)
    {
        v0 = v2;
        v3 = sub_453670(a0, a1, &g_4cf4e8.padding_0[0]);
        sub_466bc0(0);
        sub_454b00("Streming BGM Reopen %d\r\n", v3);
        return 0;
    }
    return 0xffffffff;
}
