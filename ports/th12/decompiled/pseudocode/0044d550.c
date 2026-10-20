// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44d550(void)
{
    HGDIOBJ *v1;  // esi
    HGDIOBJ v2;  // eax
    unsigned int v3;  // eax

    v2 = *(v1);
    if (!v2)
        return v2;
    v3 = DeleteObject(v2);
    *(v1) = NULL;
    return v3;
}
