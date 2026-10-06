
// block 0046ea8c
0046ea8c  cmp        dword ptr [0x4d6458], 3
0046ea93  jne        0x46eaec

// block 0046ea95
0046ea95  push       ebx
0046ea96  xor        ebx, ebx
0046ea98  cmp        dword ptr [0x4d6440], ebx

// block 0046ea9e
0046ea9e  push       edi
0046ea9f  mov        edi, dword ptr [0x4981d0]
0046eaa5  jle        0x46eada

// block 0046eaa7
0046eaa7  push       esi
0046eaa8  mov        esi, dword ptr [0x4d6444]
0046eaae  add        esi, 0x10

// block 0046eab1
0046eab1  push       0x8000
0046eab6  push       0
0046eab8  push       dword ptr [esi - 4]
0046eabb  call       dword ptr [0x49818c]

// block 0046eac1
0046eac1  push       dword ptr [esi]
0046eac3  push       0
0046eac5  push       dword ptr [0x4b3c04]
0046eacb  call       edi

// block 0046eacd
0046eacd  add        esi, 0x14
0046ead0  inc        ebx
0046ead1  cmp        ebx, dword ptr [0x4d6440]
0046ead7  jl         0x46eab1

// block 0046ead9
0046ead9  pop        esi

// block 0046eada
0046eada  push       dword ptr [0x4d6444]
0046eae0  push       0
0046eae2  push       dword ptr [0x4b3c04]
0046eae8  call       edi

// block 0046eaea
0046eaea  pop        edi
0046eaeb  pop        ebx

// block 0046eaec
0046eaec  push       dword ptr [0x4b3c04]
0046eaf2  call       dword ptr [0x498190]

// block 0046eaf8
0046eaf8  and        dword ptr [0x4b3c04], 0
0046eaff  ret        
