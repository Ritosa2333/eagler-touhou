// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461920_0 {
    unsigned int field_0;
    struct st_461920_0 *field_4;
} st_461920_0;

unsigned int sub_461920(unsigned int a0, unsigned int idx, unsigned int a2)
{
    st_461920_0 *v1;  // eax
    unsigned int v2;  // esi
    st_461920_0 *v3;  // eax
    st_461920_0 *v4;  // eax
    st_461920_0 *v5;  // eax
    unsigned int v0;  // [bp-0x4]

    if (!a2)
        return 0;
    v1 = *((int *)(idx + 8935096));
    v0 = v2;
    if (*((int *)(idx + 8935096)))
    {
        do
        {
            v3 = v1;
            if (*((int *)v3->field_0) == a2)
                return v3->field_0;
        } while ((v1 = (st_461920_0 *)v3->field_4, v3->field_4));
    }
    v4 = *((int *)(idx + 8935104));
    if (!*((int *)(idx + 8935104)))
        return 0;
    while (1)
    {
        v5 = v4;
        if (*((int *)v5->field_0) == a2)
            return v5->field_0;
        v4 = v5->field_4;
        if (!v5->field_4)
            return 0;
    }
}
