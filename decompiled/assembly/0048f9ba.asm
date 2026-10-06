
// block 0048f9ba
0048f9ba  xor        eax, eax
0048f9bc  test       dl, 0x10
0048f9bf  je         0x48f9c6

// block 0048f9c1
0048f9c1  mov        eax, 0x80

// block 0048f9c6
0048f9c6  push       ebx
0048f9c7  push       esi
0048f9c8  push       edi
0048f9c9  mov        ebx, 0x200
0048f9ce  test       dl, 8
0048f9d1  je         0x48f9d5

// block 0048f9d3
0048f9d3  or         eax, ebx

// block 0048f9d5
0048f9d5  test       dl, 4
0048f9d8  je         0x48f9df

// block 0048f9da
0048f9da  or         eax, 0x400

// block 0048f9df
0048f9df  test       dl, 2
0048f9e2  je         0x48f9e9

// block 0048f9e4
0048f9e4  or         eax, 0x800

// block 0048f9e9
0048f9e9  test       dl, 1
0048f9ec  je         0x48f9f3

// block 0048f9ee
0048f9ee  or         eax, 0x1000

// block 0048f9f3
0048f9f3  mov        edi, 0x100
0048f9f8  test       edx, 0x80000
0048f9fe  je         0x48fa02

// block 0048fa00
0048fa00  or         eax, edi

// block 0048fa02
0048fa02  mov        ecx, edx
0048fa04  mov        esi, 0x300
0048fa09  and        ecx, esi
0048fa0b  je         0x48fa2c

// block 0048fa0d
0048fa0d  cmp        ecx, edi
0048fa0f  je         0x48fa27

// block 0048fa11
0048fa11  cmp        ecx, ebx
0048fa13  je         0x48fa20

// block 0048fa15
0048fa15  cmp        ecx, esi
0048fa17  jne        0x48fa2c

// block 0048fa19
0048fa19  or         eax, 0x6000
0048fa1e  jmp        0x48fa2c

// block 0048fa20
0048fa20  or         eax, 0x4000
0048fa25  jmp        0x48fa2c

// block 0048fa27
0048fa27  or         eax, 0x2000

// block 0048fa2c
0048fa2c  mov        ecx, 0x3000000
0048fa31  pop        edi
0048fa32  and        edx, ecx
0048fa34  pop        esi
0048fa35  pop        ebx
0048fa36  cmp        edx, 0x1000000
0048fa3c  je         0x48fa54

// block 0048fa3e
0048fa3e  cmp        edx, 0x2000000
0048fa44  je         0x48fa50

// block 0048fa46
0048fa46  cmp        edx, ecx
0048fa48  jne        0x48fa59

// block 0048fa4a
0048fa4a  or         eax, 0x8000
0048fa4f  ret        

// block 0048fa50
0048fa50  or         eax, 0x40
0048fa53  ret        

// block 0048fa54
0048fa54  or         eax, 0x8040

// block 0048fa59
0048fa59  ret        
