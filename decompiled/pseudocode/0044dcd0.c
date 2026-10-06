// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44dcd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44dcd0_0 {
    char padding_0[256];
    unsigned int field_100;
    char padding_104[4];
    int field_108;
    char padding_10c[4];
    unsigned int field_110;
    char padding_114[12];
    unsigned int field_120;
} st_44dcd0_0;

char sub_44dcd0(char a0, st_44dcd0_0 *idx)
{
    int v1;  // eax
    unsigned int v2;  // esi
    unsigned int v3;  // edx
    int i;  // edi
    int j;  // edi
    int k;  // edx

    v1 = idx->field_108 * idx->field_110;
    v2 = idx->field_120;
    v3 = idx->field_100 - 21;
    if (idx->field_100 == 21)
    {
        k = 2;
        if (v1 <= 2)
            return 1;
        do
        {
            *((char *)(k + v2)) = a0;
            k += 4;
        } while (k < v1);
        return 1;
    }
    else if (v3 == 4)
    {
        j = 1;
        if (v1 <= 1)
            return 1;
        do
        {
            *((char *)(j + v2)) = *((char *)(j + v2)) & 127 | -(0 < a0) & 128;
            j += 2;
        } while (j < v1);
        return 1;
    }
    else if (v3 != 5)
    {
        return 0;
    }
    else
    {
        i = 1;
        if (v1 <= 1)
            return 1;
        do
        {
            *((char *)(i + v2)) = *((char *)(i + v2)) & 15 | a0 & 240;
            i += 2;
        } while (i < v1);
        return 1;
    }
}
