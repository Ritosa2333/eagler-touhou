// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449dc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

char * sub_449dc0(char *a0, unsigned int a1, unsigned int a2)
{
    char *iter;  // eax
    unsigned int *v2;  // edi
    unsigned int v3;  // 4126
    unsigned int v4;  // ebx
    unsigned int v5;  // ebx
    char *node;  // ecx
    char i;  // 4102
    unsigned int v8;  // ecx
    char v9;  // 4101
    char v10;  // dl
    unsigned int v0;  // [bp-0x4]

    iter = a0;
    if (*(a0) != 10)
    {
        do
        {
            if (*(iter) == 13)
                break;
            if (!*(v2))
                return iter;
            iter += 1;
            v3 = *(iter);
            *(v2) = *(v2) - 1;
        } while ((char)v3 != 10);
    }
    v0 = v4;
    v5 = *(v2);
    if (v5)
    {
        *(iter) = 0;
        node = a0;
        do
        {
            i = *(node);
            *((char *)(a2 - a0 + node)) = *(node);
            node += 1;
        } while (i);
        iter += 1;
        v8 = v5 - 1;
        while (1)
        {
            v9 = *(iter);
            v10 = *(iter);
            *(v2) = v8;
            if (v9 != 10 && v10 != 13 || !v8)
                break;
            iter += 1;
            v8 -= 1;
        }
    }
    return iter;
}
