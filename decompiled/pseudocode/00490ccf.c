// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490ccf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_490ccf_0 {
    int field_0;
} st_490ccf_0;

extern st_490ccf_0 *g_4b3d80;

int sub_490ccf(int a0)
{
    int *v0;  // esi
    int v1;  // edi

    v0 = &g_4b3d80->field_0;
    while (1)
    {
        if (!*(v0))
            return -((int)(v0 - g_4b3d80) >> 2);
        if (!sub_48e329(a0, *(v0), v1) && (*((char *)(v1 + *(v0))) == 61 || !*((char *)(v1 + *(v0)))))
            break;
        v0 += 1;
    }
    return (int)(v0 - g_4b3d80) >> 2;
}
