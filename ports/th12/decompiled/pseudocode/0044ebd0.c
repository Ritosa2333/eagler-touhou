// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ebd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_44ebd0(char *a0)
{
    unsigned int v2;  // esi
    char *v3;  // eax
    unsigned int v4;  // edi
    char *v5;  // eax
    char *v6;  // eax
    char *v7;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v2;
    v3 = a0;
    v0 = v4;
    v5 = v3;
    do
    {
        v6 = v5;
        v7 = v6 + 1;
        v5 = v7;
    } while (*(v6));
    sub_44ec80(a0, v7 - (v3 + 1));
    return;
}
