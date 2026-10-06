// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

char sub_44d020(void* idx, unsigned int a1, PSTR a2)
{
    HRSRC v4;  // edi
    HGLOBAL v5;  // ebx
    unsigned int v6;  // ebp
    unsigned int size;  // eax
    int v8;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    (*((int *)(*((int *)idx) + 4)))(v0, v1, v2, v3);
    v4 = FindResourceA(NULL, a2, 0xa);
    if (v4)
    {
        v5 = LoadResource(NULL, v4);
        if (v5)
        {
            v6 = LockResource(v5);
            if (!v6)
            {
                FreeResource(v6);
                (*((int *)(*((int *)idx) + 4)))(v0);
                return 1;
            }
            size = SizeofResource(NULL, v4);
            *((unsigned int *)&idx[4]) = size;
            v8 = sub_46d04a(size);
            *((int *)&idx[12]) = v8;
            if (v8)
            {
                sub_47a330(v8, v6, (int)idx[4]);
                *((int *)&idx[8]) = (int)idx[12];
                FreeResource(v5);
                return 1;
            }
        }
    }
    (*((int *)(*((int *)idx) + 4)))();
    return 1;
}
