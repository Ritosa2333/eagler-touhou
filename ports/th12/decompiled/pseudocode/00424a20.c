// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424a20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_424a20(void)
{
    char *v4;  // eax
    char *i;  // esi
    unsigned int v6;  // edi
    char *v7;  // edi
    int v8;  // eax
    int *v9;  // ebx
    char *v10;  // edi
    unsigned int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]
    char *v3;  // [bp+0x4]

    i = v4;
    sub_477420(v3, 0, 0x1000, v1, v2);
    if (*(v3))
        return;
    v0 = v6;
    while (1)
    {
        v7 = sub_46d1a0(i, 10);
        if (!v7)
        {
            v7 = sub_46d1a0(i, 13);
            if (!v7)
            {
                sub_47a330(v3, i, *(v9));
                *(v9) = 0;
                return;
            }
        }
        v8 = v7 - i;
        if (v8 >= 0xfff)
            v8 = 0xfff;
        sub_47a330(v3, i, v8);
        *(v9) = *(v9) + i - v7;
        for (i = v7; *(i) == 10 || *(i) == 13; *(v9) = *(v9) - 1)
        {
            i += 1;
        }
        v10 = sub_46d1a0(v3, 35);
        if (v10)
        {
            sub_477420(v10, 0, v3 - v10 + 0x1000);
            *(v10) = 0;
        }
        sub_424b50();
        if (*(v3))
            return;
    }
}
