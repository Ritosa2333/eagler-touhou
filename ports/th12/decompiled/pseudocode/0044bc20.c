// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bc20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_44bc20(void)
{
    char *v2;  // edi
    char *v3;  // eax
    char *v4;  // eax
    char *v5;  // eax
    char *v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // esi
    char *iter;  // ecx
    char i;  // 4102
    unsigned int v0;  // [bp-0x4]

    v3 = v2;
    v4 = v3;
    do
    {
        v5 = v4;
        v6 = v5 + 1;
        v4 = v6;
    } while (*(v5));
    v7 = sub_46d04a(v6 - (v3 + 1) + 1);
    if (!v7)
        return v7;
    v0 = v8;
    iter = v2;
    do
    {
        i = *(iter);
        *((char *)(v7 - v2 + iter)) = *(iter);
        iter += 1;
    } while (i);
    return v7;
}
