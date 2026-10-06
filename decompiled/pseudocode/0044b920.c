// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b920_1 {
    int field_0;
} st_44b920_1;

typedef struct st_44b920_0 {
    struct st_44b920_1 *field_0;
    int field_4;
} st_44b920_0;

int sub_44b920(void)
{
    st_44b920_0 *v2;  // eax
    int *v3;  // edi
    unsigned int v4;  // esi
    int v5;  // esi
    int v6;  // ebx
    unsigned int v0;  // [bp-0x8]

    v3 = &v2->field_0->field_0;
    if (!v2->field_0)
        return;
    v0 = v4;
    v5 = v2->field_4;
    if (v2->field_4 <= NULL)
        return;
    while (sub_46e3f8(v6, *(v3)))
    {
        v5 -= 1;
        v3 += 4;
        if (v5 <= NULL)
            return;
    }
    return;
}
