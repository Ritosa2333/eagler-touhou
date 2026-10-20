// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e700; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47e700(void* idx, unsigned int a1, int a2, int a3)
{
    int v8;  // eax
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    char *iter;  // edi
    unsigned int v12;  // ebx
    char v14;  // cl
    int v15;  // edx
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x38]
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x28]
    char v4;  // [bp-0x21]
    unsigned int v5;  // [bp-0xe]
    unsigned short v6;  // [bp-0xc]

    v8 = a3;
    v2 = v9;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    v1 = v10;
    iter = (char *)&v5 + 3;
    *((unsigned int *)idx) = 0;
    v6 = _INSERT(*((unsigned short *)((void*)&v5 + 1)), 1, 0);
    v4 = 0;
    if (v8 <= 0 && (v8 < 0 || a2 < 0))
    {
        v4 = 1;
        v8 = -(v8 + (0 < a2));
        a2 = -(a2);
    }
    v0 = v12;
    while (1)
    {
        iter -= 1;
        a2 = sub_47c890(a2, v8, 10, 0);
        *(iter) = v14 + 48;
        v8 = v15;
        v3 = v12;
        if (!a2 && !v8)
            break;
    }
    if (v4 != (char)(a2 | v8))
    {
        iter -= 1;
        *(iter) = 45;
    }
    sub_47e4f1(iter, (char *)&v6 + 1 - iter);
    return v16;
}
