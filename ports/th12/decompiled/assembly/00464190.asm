
// block 00464190
00464190  push       ecx
00464191  push       ebx
00464192  mov        ebx, dword ptr [0x4ae590]
00464198  cmp        ebx, -1
0046419b  jne        0x4641a2

// block 0046419d
0046419d  xor        eax, eax
0046419f  pop        ebx
004641a0  pop        ecx
004641a1  ret        

// block 004641a2
004641a2  push       esi
004641a3  push       edi
004641a4  call       0x46d04a

// block 004641a9
004641a9  mov        esi, eax
004641ab  add        esp, 4
004641ae  test       esi, esi
004641b0  jne        0x4641bf

// block 004641b2
004641b2  push       ebx
004641b3  call       dword ptr [0x4980a4]

// block 004641b9
004641b9  pop        esi
004641ba  xor        eax, eax
004641bc  pop        ebx
004641bd  pop        ecx
004641be  ret        

// block 004641bf
004641bf  push       0
004641c1  lea        eax, [esp + 0xc]
004641c5  push       eax
004641c6  push       edi
004641c7  push       esi
004641c8  push       ebx
004641c9  call       dword ptr [0x4980a8]

// block 004641cf
004641cf  mov        eax, esi
004641d1  pop        esi
004641d2  pop        ebx
004641d3  pop        ecx
004641d4  ret        
