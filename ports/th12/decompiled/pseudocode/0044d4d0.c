// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d4d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44d4d0(unsigned int a0, char *a1, unsigned int a2)
{
    unsigned int v1;  // esi
    char *v2;  // eax
    char *v3;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = v1;
    v2 = a1;
    do
    {
        v3 = v2;
        v2 = v3 + 1;
    } while (*(v3));
    return sub_44ec80(a1, v3 + 1 - (a1 + 1));
}
