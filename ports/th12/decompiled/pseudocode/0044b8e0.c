// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b8e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b8e0_1 {
    int field_0;
} st_44b8e0_1;

typedef struct st_44b8e0_0 {
    struct st_44b8e0_1 *field_0;
    int field_4;
} st_44b8e0_0;

int sub_44b8e0(void)
{
    st_44b8e0_0 *v1;  // eax
    int *v2;  // edi
    int v3;  // esi
    int v4;  // ebx

    v2 = &v1->field_0->field_0;
    if (!v1->field_0)
        return;
    v3 = v1->field_4;
    if (v1->field_4 <= NULL)
        return;
    while (sub_46e3f8(v4, *(v2)))
    {
        v3 -= 1;
        v2 += 4;
        if (v3 <= NULL)
            return;
    }
    return;
}
