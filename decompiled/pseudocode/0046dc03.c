// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46dc03; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46dc03_0 {
    char padding_0[3];
    char field_3;
} st_46dc03_0;


unsigned int sub_46dc03(st_46dc03_0 *a0, void* iter, unsigned int a2)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // edi
    unsigned int v4;  // edi
    st_46dc03_0 *v5;  // eax
    unsigned int v8;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int node;  // [bp-0x8]

    node = v2;
    node = 0;
    if (!a2)
        return 0;
    v0 = v3;
    if (a2 >= 4 && !(v4 = a2 - 4, !v4))
    {
        do
        {
            v5 = a0 + 1;
            iter += 4;
            if (!a0->padding_0[0] || a0->padding_0[0] != *((char *)iter - 4))
            {
                return v8;
            }
            if (!*(&v5->padding_0[0] - 3) || *(&v5->padding_0[0] - 3) != *((char *)iter - 3))
            {
                return v8;
            }
            if (!*(&v5->padding_0[0] - 2) || *(&v5->padding_0[0] - 2) != *((char *)iter - 2))
            {
                return v8;
            }
            if (!*(&v5->padding_0[0] - 1) || *(&v5->padding_0[0] - 1) != *((char *)iter - 1))
            {
                return v8;
            }
            node += 4;
            a0 = v5;
        } while (node < v4);
    }
    else
    {
        v5 = a0;
    }
    while (1)
    {
        if (node >= a2)
            return 0;
        if (!v5->padding_0[0] || v5->padding_0[0] != *((char *)iter))
            break;
        v5 = &v5->padding_0[1];
        iter += 1;
        node += 1;
    }
    return v8;
}
