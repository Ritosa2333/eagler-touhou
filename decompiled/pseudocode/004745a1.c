// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4745a1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4745a1(int a0, int a1, char *a2)
{
    int v1;  // esi
    int v2;  // ebx
    char *v3;  // eax
    char *v4;  // eax
    char v0;  // [bp+0x0]

    if (sub_47a2b9(a0, a1, a2, v1, v2, &v0))
        sub_470dc6(0, 0, 0, 0, 0);
    v3 = a2 + 64;
    if (*(v3))
        sub_474438(v3, a1, 2, "_", v3);
    v4 = a2 + 128;
    if (*(v4))
        sub_474438(2, ".", 2, ".", v4);
    return;
}
