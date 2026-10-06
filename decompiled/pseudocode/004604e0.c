// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4604e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4604e0_2 {
    unsigned int field_0;
} st_4604e0_2;

typedef struct st_4604e0_0 {
    unsigned int field_0;
    int field_4;
} st_4604e0_0;

typedef struct st_4604e0_1 {
    char padding_0[8];
    struct st_4604e0_2 *field_8;
} st_4604e0_1;

void sub_4604e0(void)
{
    unsigned int v1;  // ebx
    int *idx;  // edi
    int v3;  // esi
    int v4;  // ebp
    int i;  // ebp
    st_4604e0_1 **v6;  // eax
    st_4604e0_0 *v7;  // esi

    v1 = 0;
    if (!idx[66])
        return;
    sub_461be0(v3, v4);
    i = 0;
    if (idx[67] > 0)
    {
        do
        {
            v6 = *((int *)(idx[72] + v1));
            v7 = idx[72] + v1;
            if (v6)
            {
                *(v6)->field_8(v6);
                v7->field_0 = 0;
            }
            if (v7->field_4)
            {
                sub_46c941(v7->field_4);
                v7->field_4 = 0;
            }
            i += 1;
            v1 += 20;
        } while (i < idx[67]);
    }
    if (idx[72])
    {
        sub_46c941(idx[72]);
        idx[72] = 0;
    }
    if (idx[70])
    {
        sub_46c941(idx[70]);
        idx[70] = 0;
    }
    if (idx[71])
    {
        sub_46c941(idx[71]);
        idx[71] = 0;
    }
    if (idx[77])
    {
        sub_46c941(idx[77]);
        idx[77] = 0;
    }
    if (!idx[66])
        return;
    sub_46c941(idx[66]);
    idx[66] = 0;
    return;
}
