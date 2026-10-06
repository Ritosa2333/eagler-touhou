// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fbfe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_48fbfe(void)
{
    unsigned int v8;  // ecx
    unsigned int v9;  // sseround
    unsigned int v18;  // ecx
    unsigned int v19;  // ecx
    unsigned int v20;  // 4122
    unsigned int v21;  // edx
    unsigned int v23;  // ecx
    unsigned int v24;  // edx
    unsigned int v25;  // edx
    unsigned int v10;  // 4139
    unsigned int v11;  // ecx
    unsigned int v12;  // eax
    unsigned int v13;  // ebx
    unsigned int v14;  // esi
    unsigned int v15;  // edi
    unsigned int v16;  // edx
    unsigned int v17;  // edx
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    int v5;  // [bp-0x8], Other Possible Types: unsigned int
    unsigned int v6;  // [bp+0x4]
    unsigned int v7;  // [bp+0x8]

    v5 = v8;
    v4 = v8;
    v7 &= 50856735;
    v10 = _ccall(v9 & 3);
    v4 = v10;
    v11 = v4;
    v12 = 0;
    if ((char)v11 < 0)
        v12 = 16;
    v3 = v13;
    v2 = v14;
    v1 = v15;
    if (v11 & 0x200)
        v12 |= 8;
    if (v11 & 0x400)
        v12 |= 4;
    if (v11 & 0x800)
        v12 |= 2;
    if (v11 & 0x1000)
        v12 |= 1;
    if (v11 & 0x100)
        v12 |= 0x80000;
    v16 = v11;
    v17 = v16 & 0x6000;
    if (v16 & 0x6000)
    {
        if (v17 == 0x2000)
        {
            v12 |= 0x100;
        }
        else if (v17 == 0x4000)
        {
            v12 |= 0x200;
        }
        else if (v17 == 0x6000)
        {
            v12 |= 0x300;
        }
    }
    v18 = v11 & 32832;
    v19 = v18 - 64;
    if (v18 == 64)
    {
        v12 |= 0x2000000;
    }
    else if (v19 == 0x7fc0)
    {
        v12 |= 0x3000000;
    }
    else if (v19 == 0x8000)
    {
        v12 |= 0x1000000;
    }
    if ((~(v7) & v12 | v6 & v7) == v12)
        return;
    v5 = sub_48f9ba();
    sub_491220(v5);
    v20 = _ccall(v9 & 3);
    v5 = v20;
    v21 = v5;
    if ((char)v21 < 0)
        v0 = 16;
    v23 = v21 & 0x6000;
    v24 = v21 & 32832;
    v25 = v24 - 64;
    if (v24 == 64)
    {
        return;
    }
    else if (v25 != 0x7fc0)
    {
        return;
    }
    else
    {
        return;
    }
}
