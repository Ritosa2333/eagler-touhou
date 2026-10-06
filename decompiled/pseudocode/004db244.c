// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db244; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4db244(char a0, unsigned int a1)
{
    unsigned int v2;  // esi
    unsigned int v3;  // 4101
    unsigned int v4;  // ebx
    unsigned int v5;  // 4132
    unsigned long long v6;  // 4133
    unsigned int v7;  // 4143
    char v0;  // [bp+0x0]

    *((char *)(v2 - 1212806408)) = *((char *)(v2 - 1212806408)) | a0;
    v3 = *((int *)(v2 + a1 * 4 - 67));
    esp = &v0 & *((int *)(v2 + a1 * 4 - 67));
    *((unsigned int *)(esp - 4)) = v4;
    v5 = _ccall(15, esp & v3, 0, 0);
    v6 = _ccall(*((int *)(v2 - 2075748315)), 1, v5, 4);
    *((unsigned int *)(v2 - 2075748315)) = v6;
    *((unsigned int *)(esp - 8)) = esp - 4;
    v7 = _ccall(8, 0, (unsigned int)(v6 >> 32), 0, 0);
    if (v7 & 1)
        goto LABEL_0x4db2bf;
    else
        goto LABEL_0x4db25b;
}
