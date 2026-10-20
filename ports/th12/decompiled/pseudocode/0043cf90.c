// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cf90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_43cf90(void)
{
    unsigned int *v1;  // ebx
    int v2;  // edi
    int v3;  // esi
    int v4;  // ecx
    int v5;  // edi

    sub_477420(v1, 0, 126460, v2, v3, v4);
    *(v1) = sub_463c10(&v4, 1);
    sub_43ceb0(&v4, 1);
    v5 = v1 + 2 - v1 - 8;
    if (((unsigned int)((int)(245591161 * v5 >> 42)) >> 31) + (int)(245591161 * v5 >> 42) < 7)
    {
        do
        {
            sub_43ce00();
            v5 += 17908;
        } while (((unsigned int)((int)(245591161 * v5 >> 42)) >> 31) + (int)(245591161 * v5 >> 42) < 7);
    }
    sub_43d140(v1);
    return v1;
}
