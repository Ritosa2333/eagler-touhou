// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f4b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f4b0_0 {
    int field_0;
} st_44f4b0_0;

extern st_44f4b0_0 *g_4ce8cc;

void sub_44f4b0(unsigned int a0)
{
    int *iter;  // esi
    unsigned int v1;  // edi
    unsigned int v2;  // edi

    iter = &g_4ce8cc->field_0;
    v1 = 4;
    do
    {
        v2 = v1;
        if (*(iter) >= 0)
        {
            sub_4609d0();
            *(iter) = -0x1;
        }
    } while ((iter += 40, v1 = v2 - 1, v2 != 1));
    return;
}
