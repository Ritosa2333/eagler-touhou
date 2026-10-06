// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493b07; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52d0;
extern int g_4d52d8;

unsigned int * sub_493b07(unsigned int a0, unsigned int a1, unsigned long long *a2, int a3)
{
    unsigned int v15;  // edi
    int v25;  // ecx
    int v26;  // ecx
    unsigned int *v17;  // eax
    int v18;  // edx
    int v19;  // edx
    int v20;  // edx
    int v21;  // edx
    int v22;  // edx
    int v23;  // ecx
    int v24;  // ecx
    char *v0;  // [bp-0x3c], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x28]
    unsigned long v4;  // [bp-0x24]
    unsigned long v5;  // [bp-0x1c]
    double v6;  // [bp-0x14], Other Possible Types: unsigned long
    char v7;  // [bp-0xc]
    char v8;  // [bp-0xb]
    char v9;  // [bp-0xa]
    char v10;  // [bp-0x9]
    char v11;  // [bp-0x8]
    char v12;  // [bp-0x7]
    char v13;  // [bp-0x6]
    char v14;  // [bp-0x5]

    v1 = v15;
    v7 = 0;
    v8 = 0;
    v9 = 0;
    v10 = 0;
    v11 = 0;
    v12 = 0;
    v13 = 0;
    v14 = 0;
    v17 = (!g_4d52d0 ? sub_49616c : sub_4751de(g_4d52d8));
    if (a3 <= 166)
    {
        if (a3 == 166)
        {
            v2 = 3;
            v3 = "exp10";
LABEL_493bb8:
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v17 = v17(&v2);
            if (!v17)
            {
                v17 = sub_46e96f();
                *(v17) = 0x22;
                goto LABEL_493d79;
            }
        }
        if (a3 <= 25)
        {
            if (a3 == 25)
            {
                v3 = "pow";
LABEL_493bed:
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = 4;
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v17 = v17(&v2);
                goto LABEL_493d79;
            }
            v0 = 2;
            v18 = a3 - 2;
            if (a3 != 2)
            {
                v19 = v18 - 1;
                if (v18 != 1)
                {
                    v20 = v19 - 5;
                    if (v19 != 5)
                    {
                        v21 = v20 - 1;
                        if (v20 != 1)
                        {
                            v22 = v21 - 5;
                            if (v21 == 5)
                            {
                                v2 = 3;
                                v3 = "exp";
                                goto LABEL_493bb8;
                            }
                            else if (v22 == 1)
                            {
                                v3 = "exp";
                                goto LABEL_493bed;
                            }
                            else if (v22 == 10)
                            {
                                v2 = 3;
                                goto LABEL_493bb1;
                            }
                            else
                            {
                                return v17;
                            }
                        }
                        else
                        {
                            v3 = "log10";
                            goto LABEL_493c26;
                        }
                    }
                    else
                    {
                        v2 = 2;
                        v3 = "log10";
                        goto LABEL_493bb8;
                    }
                }
                else
                {
                    v3 = "log";
                    goto LABEL_493c26;
                }
            }
            else
            {
                v2 = 2;
                v3 = "log";
                goto LABEL_493bb8;
            }
        }
        else
        {
            v23 = a3 - 26;
            if (a3 == 26)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                *(a2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return v17;
            }
            v24 = v23 - 1;
            if (v23 != 1)
            {
                v25 = v24 - 1;
                if (v24 == 1)
                    goto LABEL_493ca2;
                v26 = v25 - 1;
                if (v25 != 1)
                {
                    if (v26 == 29)
                        goto LABEL_493c8c;
                    if (v26 != 32)
                        return v17;
                    goto LABEL_493c83;
                }
                else
                {
                    v3 = "pow";
                }
            }
            else
            {
                v2 = 2;
LABEL_493bb1:
                v3 = "pow";
                goto LABEL_493bb8;
            }
        }
    }
    else
    {
        switch (a3)
        {
        case 1000:
            v3 = "log";
            break;
        case 1001:
            v3 = "log10";
            break;
        case 1002:
            v3 = "exp";
            break;
        case 1003:
            v3 = "atan";
            break;
        case 1004:
            v3 = "ceil";
            break;
        case 1005:
            v3 = "floor";
            break;
        case 1006:
LABEL_493ca2:
            v3 = "pow";
LABEL_493c26:
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_493d59;
        case 1007:
            v3 = "modf";
            break;
        case 1008:
LABEL_493c8c:
            v3 = "acos";
            goto LABEL_493c26;
        case 1009:
LABEL_493c83:
            v3 = "asin";
            goto LABEL_493c26;
        case 1010:
            v3 = "sin";
            goto LABEL_493d48;
        case 1011:
            v3 = "cos";
            goto LABEL_493d48;
        case 1012:
            v3 = "tan";
LABEL_493d48:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *(a2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
LABEL_493d59:
            if (/* unsupported instruction */)
            {
                v6 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v6 = (double)nan;
                /* unsupported instruction */
            }
            v0 = &v2;
            v2 = 1;
            v17 = v17();
            if (!v17)
            {
                v17 = sub_46e96f();
                *(v17) = 33;
            }
LABEL_493d79:
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                *(a2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return v17;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *(a2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return v17;
        default:
            return v17;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    *(a2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    goto LABEL_493c26;
}
