// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e975; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e975_0 {
    char field_0;
} st_47e975_0;

extern st_47e975_0 *g_4b4318;

int sub_47e975(void)
{
    unsigned int v2;  // edx
    unsigned int v0;  // [bp-0x8]
    void* v1;  // [bp+0x4]

    if (!g_4b4318->field_0)
    {
        v0 = 1;
    }
    else if (g_4b4318->field_0 != 65)
    {
        v0 = 2;
    }
    else
    {
        g_4b4318 = g_4b4318 + 1;
        sub_47e59a("{flat}");
        return;
    }
    sub_47e1ce(v1, v2, v0);
    return;
}
