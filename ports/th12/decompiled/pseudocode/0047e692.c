// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e692; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_47e692(void* idx, unsigned int a1, unsigned long long a2, unsigned int a3)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    char *iter;  // edi
    unsigned int v8;  // edi
    char v10;  // cl
    unsigned int v11;  // edx
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x24]
    int v3;  // [bp-0xe]

    v1 = v5;
    v0 = v6;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    iter = &v3 + 2;
    *((unsigned int *)idx) = 0;
    *((char *)&(&v3)[2]) = 0;
    do
    {
        iter -= 1;
        a2 = (unsigned int)sub_47c890(a2, a3, 10, 0, v8);
        v2 = v5;
        *(iter) = v10 + 48;
        a3 = v11;
    } while (a2 || a3);
    sub_47e4f1(iter, &v3 + 2 - iter);
    return v12;
}
