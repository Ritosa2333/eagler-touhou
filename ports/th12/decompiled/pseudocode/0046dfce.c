// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46dfce; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46dfce_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46dfce_0;

extern char g_49cd28;

void* sub_46dfce(void* idx, unsigned int a1, st_46dfce_0 *index)
{
    unsigned int v1;  // eax
    int v2;  // eax
    unsigned int v3;  // edi
    unsigned int v5;  // edi
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x10]

    *((char **)idx) = &g_49cd28;
    v1 = index->field_8;
    *((unsigned int *)&idx[8]) = v1;
    v2 = index->field_4;
    v0 = v3;
    if (!v1)
    {
        *((int *)&idx[4]) = v2;
        return idx;
    }
    else if (v2)
    {
        v5 = sub_475860(v2) + 1;
        v6 = sub_46d04a(v5);
        *((unsigned int *)&idx[4]) = v6;
        if (!v6)
            return idx;
        sub_47a2b9(v6, v5, index->field_4);
        return idx;
    }
    else
    {
        *((unsigned int *)&idx[4]) = 0;
        return idx;
    }
}
