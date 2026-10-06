// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466350_1 {
    unsigned int field_0;
} st_466350_1;

typedef struct st_466350_0 {
    char padding_0[48];
    struct st_466350_1 *field_30;
} st_466350_0;

int sub_466350(void)
{
    unsigned int v3;  // edi
    unsigned int *idx;  // eax
    int v5;  // esi
    st_466350_0 **v6;  // esi
    int v7;  // ebx
    unsigned int v8;  // ecx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0xc]

    v1 = v3;
    if (!idx[1])
        return;
    v6 = sub_466280(v5);
    if (!v6)
    {
        return;
    }
    else if (sub_466220(v7) >= 0)
    {
        if (v8)
        {
            v0 = 0;
            if (sub_465fe0(v6) < 0)
                return;
            sub_4665c0();
        }
        memset(idx + 5, 0, 12);
        sub_4663e0();
        idx[12] = 1;
        idx[8] = 0;
        idx[9] = 1;
        idx[11] = 0;
        *(v6)->field_30(v6, 0, 0, 1);
        return;
    }
    else
    {
        return;
    }
}
