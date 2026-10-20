// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f89d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f89d_0 {
    char field_0;
} st_47f89d_0;

extern st_47f89d_0 *g_4b4318;

int sub_47f89d(void)
{
    char *v7;  // eax
    unsigned int v8;  // edx
    int v9;  // esi
    unsigned int *v10;  // eax
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0x1c]
    char *v1;  // [bp-0x18]
    char v2;  // [bp-0x14]
    char v3;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int *v4;  // [bp+0x4]
    unsigned int *v5;  // [bp+0x8]
    unsigned int v6;  // [bp+0xc]

    v7 = &g_4b4318->field_0;
    if (!*(v7))
    {
        sub_47ec26(v4, 1, v5);
    }
    else if (v6 && *(v7) == 88)
    {
        g_4b4318 = v7 + 1;
        if (!*(v5))
        {
            sub_47e59a(v4, v8, "void");
            return;
        }
        sub_47ec4a(v4, "void ", v5);
    }
    else if (*(v7) == 89)
    {
        g_4b4318 = v7 + 1;
        sub_47f1b4(v4, v5);
        return;
    }
    else
    {
        sub_4822f0(&v3, v5, v9);
        if (v5[1] & 0x4000)
        {
            v1 = &v3;
            v0 = "cli::array<";
            goto LABEL_47f92e;
        }
        else if (v5[1] & 0x2000)
        {
            v1 = &v3;
            v0 = "cli::pin_ptr<";
LABEL_47f92e:
            v10 = sub_47ec4a(&v2);
            v3 = *(v10);
            v11 = v10[1];
        }
        *(v4) = v3;
        v4[1] = v11;
        return;
    }
    return;
}
