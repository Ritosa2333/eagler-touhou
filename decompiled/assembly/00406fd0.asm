
// block 00406fd0
00406fd0  mov        ecx, dword ptr [0x4b43cc]
00406fd6  mov        eax, dword ptr [ecx + 0x7c]
00406fd9  test       al, 1
00406fdb  je         0x407006

// block 00406fdd
00406fdd  cmp        dword ptr [ecx + 0x28], 0x3c
00406fe1  jl         0x406ff4

// block 00406fe3
00406fe3  and        eax, 0xffffffdd
00406fe6  mov        dword ptr [ecx + 0x80], 0
00406ff0  mov        dword ptr [ecx + 0x7c], eax
00406ff3  ret        

// block 00406ff4
00406ff4  mov        edx, dword ptr [0x4b43c4]
00406ffa  cmp        dword ptr [edx + 0x3c], 0
00406ffe  je         0x407006

// block 00407000
00407000  or         eax, 0x20
00407003  mov        dword ptr [ecx + 0x7c], eax

// block 00407006
00407006  ret        
