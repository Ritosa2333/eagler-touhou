// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47a12b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b41d4;
extern char g_4b41dc;

int sub_47a12b(int a0, unsigned int a1, int a2, int a3, void* a4)
{
    unsigned int v4;  // edi
    char *v5;  // edi
    int v6;  // esi
    int v7;  // eax
    char *v8;  // edi
    unsigned int v9;  // eax
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0xa4]
    int v1;  // [bp-0x94]
    unsigned int v2;  // [bp-0x8c]
    char v3;  // [bp-0x88]

    v0 = v4;
    if (a1 == 1)
    {
        v5 = &v3;
        v2 = 0;
        v6 = sub_48c424(a0, a2, a3, &v3, 128, 0);
        if (!v6)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && (v1 = (int)sub_48c424(a0, a2, a3, 0, 0, 0), v1 && (v5 = (char *)sub_477813(v1, 1), v5)))
            {
                v2 = 1;
                v6 = sub_48c424(a0, a2, a3, v5, v1, 0);
                if (v6)
                    goto LABEL_47a1f5;
LABEL_47a213:
                sub_46c941(v5);
            }
        }
        else
        {
LABEL_47a1f5:
            v7 = sub_477813(v6, 1);
            *((int *)a4) = v7;
            if (!v7)
            {
                if (v2)
                    goto LABEL_47a213;
            }
            else
            {
                if (sub_4830fd(v7, v6, v5, v6 - 1))
                    sub_470dc6(0, 0, 0, 0, 0);
                if (v2)
                {
                    sub_46c941(v5);
                    return v11;
                }
                goto LABEL_47a25b;
            }
        }
    }
    else if (!a1 && (v8 = &g_4b41d4, sub_48c2b1(a0, a2, a3, &g_4b41d4, 4, 0)))
    {
        *((char *)a4) = 0;
        do
        {
        } while (sub_48ba71(*(v8)) && (*((char *)a4) = *((char *)(void*)&*((char *)a4) * 10) + *(v8) - 48, v8 += 2, v8 < &g_4b41dc));
LABEL_47a25b:
        return v11;
    }
    return v9;
}
