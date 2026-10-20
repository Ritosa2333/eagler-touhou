// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453620; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453620_0 {
    char padding_0[4];
    unsigned int field_4;
} st_453620_0;

int sub_453620(void)
{
    unsigned int v1;  // eax
    unsigned int v2;  // ebx
    st_453620_0 *v3;  // esi
    unsigned int v4;  // ebx
    unsigned int *v5;  // edi
    unsigned int v6;  // ecx
    int v0;  // [bp+0x4]

    v2 = v1;
    if (!v2)
        return;
    while (1)
    {
        v4 = v2;
        *(v5) = v3->field_4;
        if (!sub_46dc03(v3, v0, 4))
            return;
        v6 = 0xfffffff8 - *(v5);
        v2 = v4 + v6;
        v3 = &v3->padding_0[*(v5) + 8];
        if (!(v4 + v6))
            return;
    }
}
