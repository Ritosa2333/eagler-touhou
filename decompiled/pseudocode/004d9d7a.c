// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9d7a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4912847a;

int sub_4d9d7a(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // ebx
    unsigned int v5;  // 4118
    unsigned int v6;  // id
    unsigned int v7;  // ac
    unsigned int *v8;  // ecx
    unsigned int v9;  // cc_dep1
    unsigned int v10;  // 4103
    unsigned int v0;  // [bp+0x0]
    unsigned int v1;  // [bp+0x0]

    v5 = _ccall(6, *((int *)(v3 - 1784338505)), v4, 0);
    v0 = v5 | 0x202 | v6 * 0x200000 & 0x200000 | v7 * 0x40000 & 0x40000;
    *(v8) = *(v8) ^ v1;
    v9 = *(v8) ^ v1;
    if (v8 != 0x1)
        goto LABEL_0x4d9dc0;
    g_4912847a = v1;
    v10 = _ccall(0, 15, v9, 0, 0);
    if (!(v10 & 1))
        goto LABEL_0x4d9d22;
}
