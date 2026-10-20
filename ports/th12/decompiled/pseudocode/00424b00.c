// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424b00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_424b00(void)
{
    char *v1;  // eax
    char *iter;  // eax
    char i;  // 4102
    int v4;  // edi
    char *v5;  // eax
    char *v6;  // ecx
    char *node;  // ecx
    char j;  // 4102
    unsigned int v9;  // ebx

    iter = v1;
    do
    {
        i = *(iter);
        *((char *)(v4 - v1 + iter)) = *(iter);
        iter += 1;
    } while (i);
    v5 = sub_46d1a0(v4, 58);
    if (!v5)
    {
        sub_424b50();
        return;
    }
    v6 = v5 + 1;
    node = v6;
    do
    {
        j = *(node);
        *((char *)(v9 - v6 + node)) = j;
        node += 1;
    } while (j);
    *(v5) = j;
    sub_424b50();
    sub_424b50();
    return;
}
