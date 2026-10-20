// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4241b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4241b0(int a0)
{
    int idx;  // esi
    unsigned int v1;  // edi
    unsigned int v10;  // 4111
    unsigned int v11;  // 4120
    char *v2;  // ecx
    char *v3;  // ebx
    char *iter;  // eax
    char v5;  // 4101
    unsigned int v6;  // cc_dep1
    unsigned int v7;  // cc_dep2
    char v8;  // 4103
    unsigned int v9;  // eax

    idx = 0;
    if (a0 <= 0)
        return 0;
    while (1)
    {
        v2 = *((int *)(v1 + idx * 8));
        iter = v3;
        while (1)
        {
            v5 = *(iter);
            v6 = v5;
            v7 = *(v2);
            if (v5 != *(v2))
                break;
            if (v5)
            {
                v8 = iter[1];
                v6 = v8;
                v7 = v2[1];
                if (v8 != v2[1])
                    break;
                iter += 2;
                v2 += 2;
                if (!v8)
                    goto LABEL_4241e1;
            }
            else
            {
LABEL_4241e1:
                v9 = 0;
                goto LABEL_4241ea;
            }
        }
        v10 = _ccall(4, v6, v7, 0);
        v11 = _ccall(12, 0, v10 & 1, v10 & 1);
        v9 = -(v10 & 1) + 1 - (v11 & 1);
LABEL_4241ea:
        if (!v9)
            return *((int *)(v1 + idx * 8 + 4));
        idx += 1;
        if (idx >= a0)
            return 0;
    }
}
