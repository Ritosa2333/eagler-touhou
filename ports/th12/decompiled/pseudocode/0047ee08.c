// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ee08; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ee08_0 {
    char field_0;
} st_47ee08_0;

extern st_47ee08_0 *g_4b4318;

int sub_47ee08(void)
{
    unsigned int v5;  // edx
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // edx
    unsigned int v10;  // ecx
    unsigned int *v11;  // eax
    char *v0;  // [bp-0x18], Other Possible Types: int
    char v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int *v4;  // [bp+0x4]

    v2 = 0;
    v3 &= 0xffff0000;
    if (!g_4b4318->field_0)
    {
        v0 = 1;
        sub_47e1ce(v0);
        return;
    }
    switch (g_4b4318->field_0)
    {
    case 48: case 49:
        v0 = "char ";
        break;
    case 50: case 51:
        v0 = "short ";
        break;
    case 53:
        v0 = "int ";
        break;
    case 54: case 55:
        v0 = "long ";
        sub_47e896(&v2, v5, v0);
    case 52:
        v6 = g_4b4318->field_0;
        g_4b4318 = g_4b4318 + 1;
        if (v6 == 49 || (v7 = v6 - 50, v7 == 1 || (v8 = v7 - 2, v7 == 3 || v7 == 5)))
        {
            v11 = sub_47ec4a(&v1, "unsigned ", &v2);
            v10 = *(v11);
            v9 = v11[1];
        }
        else
        {
            v9 = v3;
            v10 = v2;
        }
        *(v4) = v10;
        v4[1] = v9;
        return;
    default:
        v0 = 2;
        sub_47e1ce(v0);
        return;
    }
}
