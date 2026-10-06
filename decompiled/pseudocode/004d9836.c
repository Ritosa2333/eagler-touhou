// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9836; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9836(void)
{
    char *v1;  // eax
    char v2;  // bl
    char v3;  // ch
    char *v4;  // esi
    unsigned int v5;  // ecx
    unsigned int v6;  // ecx
    char *v7;  // edi
    char v8;  // bh

    *(v1) = *(v1) | v2;
    v5 = _INSERT(0, 1, v3 ^ v4[37]);
    v6 = _INSERT(v5, 0, 212);
    *((char *)(v6 - 922775495)) = *((char *)(v6 - 922775495)) - (v3 ^ v4[37]);
    *(v7) = *(v7) + 19;
    if (v8 < *(v4))
        goto LABEL_0x4d981b;
    return;
}
