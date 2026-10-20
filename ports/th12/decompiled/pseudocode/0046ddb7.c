// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ddb7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_46ddb7(PSTR a0)
{
    char *v10;  // esi
    unsigned int v11;  // eax
    unsigned int v12;  // edi
    unsigned int ch;  // edi
    unsigned int v14;  // ebx
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x128]
    int v1;  // [bp-0x124]
    int v2;  // [bp-0x120]
    unsigned int v3;  // [bp-0x11c]
    unsigned int v4;  // [bp-0x118]
    Byte v5;  // [bp-0x114]
    char v6;  // [bp-0x113]
    char v7;  // [bp-0x112]
    char v8;  // [bp-0x111]
    Byte v9[264];  // [bp-0x110]

    v10 = &v9[0];
    v3 = 0;
    if (!a0)
    {
        *((unsigned int *)sub_46e982(v1, v2, 0, -0x1)) = 0;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return v11;
    }
    v0 = v12;
    if (SetCurrentDirectoryA(a0))
    {
        ch = GetCurrentDirectoryA(261, v9);
        if (ch > 260)
        {
            v14 = ch + 1;
            v10 = sub_477813(v14, 1);
            if (v10 && !(v3 = 1, !ch))
            {
                ch = GetCurrentDirectoryA(v14, v10);
                goto LABEL_46de6a;
            }
            else
            {
            }
        }
        else
        {
LABEL_46de6a:
            if (ch && ((*(v10) == 92 || *(v10) == 47) && *(v10) == v10[1] || (v5 = 61, v6 = (char)sub_479fe0((unsigned int)*(v10)), v7 = 58, v8 = 0, SetEnvironmentVariableA(&v5, v10))))
            {
                v4 = 0;
                goto LABEL_46decb;
            }
        }
    }
    sub_46e995(GetLastError());
LABEL_46decb:
    if (!v3)
        return v16;
    sub_46c941(v10);
    return v16;
}
