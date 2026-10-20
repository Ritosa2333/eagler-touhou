// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d490; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

HGDIOBJ sub_44d490(void)
{
    void* idx;  // esi
    HGDIOBJ v2;  // eax

    v2 = *((int *)idx);
    if (v2)
    {
        v2 = DeleteObject(v2);
        *((HGDIOBJ *)idx) = NULL;
    }
    if ((int)idx[36] >= 16)
        v2 = sub_46ca4f((int)idx[16]);
    *((unsigned int *)&idx[36]) = 15;
    *((unsigned int *)&idx[32]) = 0;
    *((char *)&idx[16]) = 0;
    return v2;
}
