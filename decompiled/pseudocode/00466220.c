// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466220; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466220_1 {
    unsigned int field_0;
} st_466220_1;

typedef struct st_466220_0 {
    char padding_0[36];
    struct st_466220_1 *field_24;
    char padding_28[40];
    struct st_466220_1 *field_50;
} st_466220_0;


int sub_466220(void)
{
    unsigned int *v3;  // ebx
    st_466220_0 **v4;  // esi
    int v5;  // eax
    unsigned int v6;  // edi
    unsigned int v0;  // [bp-0x8]
    char v1;  // [bp-0x4]

    if (v3)
        *(v3) = 0;
    v5 = *(v4)->field_24(v4, &v1);
    if (v5 < 0)
    {
        return v5;
    }
    else if (v1 & 2)
    {
        v0 = v6;
        do
        {
            if (*(v4)->field_50(v4) == 2289565846)
                Sleep(10);
        } while (*(v4)->field_50(v4));
        if (!v3)
            return 0;
        *(v3) = 1;
        return 0;
    }
    else
    {
        return 1;
    }
}
