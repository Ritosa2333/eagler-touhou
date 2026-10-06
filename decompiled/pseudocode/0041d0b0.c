// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d0b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d0b0_0 {
    char padding_0[168];
    int field_a8;
} st_41d0b0_0;

extern st_41d0b0_0 *g_4b43cc;

unsigned int sub_41d0b0(unsigned int *a0)
{
    int v0;  // eax
    int v1;  // edx
    unsigned int v2;  // edx
    unsigned int *v3;  // ebx
    int v4;  // eax
    unsigned int *v5;  // ebx

    v0 = g_4b43cc->field_a8;
    v1 = (int)(g_4b43cc->field_a8 / 100) % 1000;
    v2 = 351843721 * g_4b43cc->field_a8 >> 45;
    if (v2 + (v2 >> 31) - 22 == (int)((v1 + 934) % 1000) + (int)(((int)(v0 % 100) + 67) % 100))
    {
        v4 = g_4b43cc->field_a8;
        *(a0) = ((int)((int)(v4 / 100) % 1000) + 934) % 1000;
        *(v5) = ((int)(v4 % 100) + 67) % 100;
        return ((int)(v4 % 100) + 67) / 100;
    }
    *(a0) = 999;
    *(v3) = 99;
    return v2 >> 31;
}
