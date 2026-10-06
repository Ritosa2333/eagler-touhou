// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e343; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48e343_0 {
    unsigned int field_0;
} st_48e343_0;

extern st_48e343_0 *g_4b3d88;

unsigned int sub_48e343(unsigned int a0)
{
    unsigned int v4;  // edi
    unsigned int *iter;  // edi
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x18]
    int v1;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x8]

    v2 = a0;
    v1 = a0;
    v0 = v4;
    iter = &g_4b3d88->field_0;
    v2 = 0;
    v6 = g_4b3d88->field_0;
    if (!g_4b3d88->field_0)
        return 0;
    do
    {
        v1 = WideCharToMultiByte(0, 0, v6, -0x1, NULL, 0, NULL, NULL);
        if (!v1)
            return 0xffffffff;
        v2 = sub_477813(v1, 1);
        if (!v2)
            return 0xffffffff;
        if (!WideCharToMultiByte(0, 0, *(iter), -0x1, v2, v1, NULL, NULL))
        {
            sub_46c941(v2);
            return 0xffffffff;
        }
        if (sub_490d7b(&v2, 0) < 0 && v2)
        {
            sub_46c941(v2);
            v2 = 0;
        }
        iter += 1;
        v6 = *(iter);
    } while (*(iter));
    return 0;
}
