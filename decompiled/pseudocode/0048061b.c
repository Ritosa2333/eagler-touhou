// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48061b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48061b_0 {
    char field_0;
    char field_1;
} st_48061b_0;

extern st_48061b_0 *g_4b4318;

int sub_48061b(unsigned int a0)
{
    char *v0;  // eax

    if (g_4b4318->field_0 == 63)
    {
        v0 = &g_4b4318->field_1;
        if (*(v0) == 36)
        {
            sub_4800f3(a0, 1);
            return a0;
        }
        g_4b4318 = v0;
        sub_47fb56(a0, 0, NULL);
    }
    else
    {
        sub_48023a(a0, 1, 0);
    }
    return a0;
}
