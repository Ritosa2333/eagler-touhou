// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484e7d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_484e7d_0 {
    char field_0;
} st_484e7d_0;

int sub_484e7d(void)
{
    char *v2;  // edx
    char *v3;  // esi
    unsigned int *v4;  // eax
    unsigned int v5;  // edi
    st_484e7d_0 **v6;  // ecx
    unsigned int v0;  // [bp-0x8]

    v3 = v2;
    if (!*(v4))
        return;
    v0 = v5;
    do
    {
    } while (*(v3) && (*(v6)->field_0 = *(v3), *(v6) = *(v6) + 1, v3 += 1, *(v4) = *(v4) - 1, *(v4)));
    return;
}
