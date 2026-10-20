// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c461; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_48c461(uint128_t *iter, uint128_t *node, unsigned int a2)
{
    unsigned int v0;  // ecx
    unsigned int i;  // ecx
    int v2;  // xmm1
    int v3;  // xmm2
    int v4;  // xmm3
    int v5;  // xmm5
    int v6;  // xmm6
    int v7;  // xmm7

    v0 = a2 >> 7;
    do
    {
        i = v0;
        v2 = (int)node[1];
        v3 = (int)node[2];
        v4 = (int)node[3];
        *(iter) = *(node);
        iter[1] = (uint128_t)v2;
        iter[2] = (uint128_t)v3;
        iter[3] = (uint128_t)v4;
        v5 = (int)node[5];
        v6 = (int)node[6];
        v7 = (int)node[7];
        iter[4] = node[4];
        iter[5] = (uint128_t)v5;
        iter[6] = (uint128_t)v6;
        iter[7] = (uint128_t)v7;
        node += 8;
        iter += 8;
        v0 = i - 1;
    } while (i != 1);
    return;
}
