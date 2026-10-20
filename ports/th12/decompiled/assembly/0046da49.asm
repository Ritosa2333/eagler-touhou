
// block 0046da49
0046da49  cmp        dword ptr [0x49d57c], 0
0046da50  push       esi
0046da51  je         0x46da68

// block 0046da53
0046da53  push       0x49d57c
0046da58  call       0x477a40

// block 0046da5d
0046da5d  pop        ecx
0046da5e  test       eax, eax
0046da60  je         0x46da68

// block 0046da62
0046da62  call       dword ptr [0x49d57c]

// block 0046da68
0046da68  call       0x4753ee

// block 0046da6d
0046da6d  mov        esi, eax
0046da6f  test       esi, esi
0046da71  je         0x46da89

// block 0046da73
0046da73  mov        eax, dword ptr [esi + 4]
0046da76  cmp        eax, -1
0046da79  je         0x46da82

// block 0046da7b
0046da7b  push       eax
0046da7c  call       dword ptr [0x4980a4]

// block 0046da82
0046da82  push       esi
0046da83  call       0x4755b0

// block 0046da88
0046da88  pop        ecx

// block 0046da89
0046da89  push       0
0046da8b  call       dword ptr [0x4981e0]

// block 0046da91
0046da91  int3       
