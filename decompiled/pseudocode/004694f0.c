// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4694f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4694f0(void)
{
    int v1;  // edi
    unsigned int *idx;  // eax
    int v11;  // eax
    unsigned int v12;  // 4111
    unsigned int v13;  // 4120
    int v3;  // ebx
    int index;  // esi
    char *v5;  // edx
    char *iter;  // ecx
    char v7;  // 4101
    unsigned int v8;  // cc_dep1
    unsigned int v9;  // cc_dep2
    char v10;  // 4103
    char *v0;  // [bp+0x4]

    v1 = 0;
    v3 = idx[2] - 1;
    if (idx[2] - 1 < 0)
        return;
    while (1)
    {
        index = (v3 - v1) / 2 + v1;
        v5 = *((int *)(idx[35] + index * 8));
        iter = v0;
        while (1)
        {
            v7 = *(iter);
            v8 = v7;
            v9 = *(v5);
            if (v7 != *(v5))
                break;
            if (v7)
            {
                v10 = iter[1];
                v8 = v10;
                v9 = v5[1];
                if (v10 != v5[1])
                    break;
                iter += 2;
                v5 += 2;
                if (!v10)
                    goto LABEL_46953c;
            }
            else
            {
LABEL_46953c:
                v11 = 0;
                goto LABEL_469545;
            }
        }
        v12 = _ccall(4, v8, v9, 0);
        v13 = _ccall(12, 0, v12 & 1, v12 & 1);
        v11 = -(v12 & 1) + 1 - (v13 & 1);
LABEL_469545:
        if (!v11)
            return;
        if (v11 < 0)
            v3 = index - 1;
        else
            v1 = index + 1;
        if (v1 > v3)
            return;
    }
}
