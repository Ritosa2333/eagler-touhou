// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48edb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48edb0(char *iter, char *node, unsigned int a2)
{
    unsigned int i;  // ecx
    char v1;  // ah
    char v2;  // al
    char v3;  // al

    if (!a2)
        return a2;
    do
    {
        i = a2;
        v1 = *(iter);
        v2 = *(node);
        v3 = v2;
        if (!*(iter) || (v3 = v2, !v2))
            break;
        iter += 1;
        node += 1;
        if (v1 >= 65 && v1 <= 90)
            v1 += 32;
        if (v3 >= 65 && v3 <= 90)
            v3 += 32;
        if (v1 != v3)
            goto LABEL_48ee01;
        a2 = i - 1;
    } while (i != 1);
    if (v1 == v3)
        return 0;
LABEL_48ee01:
    if (v1 >= v3)
        return 0;
    return 0xffffffff;
    return 0;
}
