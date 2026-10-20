// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424b50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_424b50(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    char *iter;  // edi
    char *v14;  // ecx
    char *v15;  // ecx
    char *v16;  // ecx
    char *v17;  // ecx
    int v18;  // edx
    int v19;  // edx
    char v20;  // cl
    char *v5;  // eax
    char v6;  // cl
    char *v7;  // ecx
    char *v8;  // ecx
    char *v9;  // ecx
    char *v10;  // ecx
    int v11;  // ecx
    char *i;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    while (1)
    {
        v6 = *(v5);
        if (*(v5) != 32 && v6 != 9 && v6 != 10 && v6 != 13)
            break;
        v7 = v5;
        v8 = v7;
        do
        {
            v9 = v8;
            v10 = v9 + 1;
            v8 = v10;
        } while (*(v9));
        v11 = v10 - (v7 + 1);
        if (v11 <= 0)
            continue;
        i = v5 + 1;
        for (iter = v5; v11; i += 1)
        {
            v11 -= 1;
            *(iter) = *(i);
            iter += 1;
        }
    }
    v14 = v5;
    v15 = v14;
    do
    {
        v16 = v15;
        v17 = v16 + 1;
        v15 = v17;
    } while (*(v16));
    v18 = v17 - (v14 + 1) - 1;
    if (v18 < 0)
        return;
    while (1)
    {
        v19 = v18;
        v20 = v5[v19];
        if (v5[v19] != 32 && v20 != 9 && v20 != 10 && v20 != 13)
            return;
        v18 = v19 - 1;
        v5[1 + v18] = 0;
        if (v19 - 1 < 0)
            return;
    }
}
