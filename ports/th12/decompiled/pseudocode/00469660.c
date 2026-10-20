// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469660; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_469660(void)
{
    void* idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // 4108
    unsigned int *v4;  // esi
    char v5;  // dl

    v2 = (int)idx[0x1000] - 4;
    v3 = _ccall(8, 3, (int)idx[0x1000], 0xfffffffc, 0);
    if (v3 & 1)
        return;
    *((unsigned int *)&idx[0x1000]) = v2;
    *(v4) = *((int *)(v2 + (char *)idx));
    if (!v5)
        return;
    *((unsigned int *)&idx[0x1000]) = (int)idx[0x1000] - 4;
    if (*(idx[0x1000] + (char *)idx) == 0x66)
    {
        if (v5 != 105)
            return;
        /* unsupported instruction */
        /* unsupported instruction */
        *(v4) = sub_4931e0();
        return;
    }
    else if (*(idx[0x1000] + (char *)idx) == 105)
    {
        if (v5 != 0x66)
            return;
        /* unsupported instruction */
        /* unsupported instruction */
        *(v4) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return;
    }
    else
    {
        return;
    }
}
