// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492b9e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492b9e_1 {
    int field_0;
    unsigned int field_4;
} st_492b9e_1;

typedef struct st_492b9e_0 {
    char padding_0[8];
    unsigned int field_8;
} st_492b9e_0;

void sub_492b9e(unsigned int a0, unsigned int a1, int a2, unsigned int a3, int a4, unsigned int a5, st_492b9e_0 *v0)
{
    st_492b9e_0 *v2;  // esi
    int v3;  // ebx
    st_492b9e_1 *v4;  // edi
    unsigned int v5;  // eax
    unsigned int v1;  // [bp-0x8]

    if (a4)
        sub_4929c7(a0, v2, v3, a4);
    v1 = a0;
    if (!v0)
    {
        v0 = v2;
        v0 = v2;
    }
    else
    {
        v0 = v0;
    }
    sub_491a21(v0);
    sub_49205b(v2, a2, a3, v4->field_0);
    v2->field_8 = v4->field_4 + 1;
    v5 = sub_4926ac(a0, v2, a1, a3, a5, 0x100);
    if (v5)
        sub_4919da(v5, v2);
    return;
}
