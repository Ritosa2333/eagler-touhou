
// block 00427380
00427380  mov        edx, dword ptr [0x4b44e8]
00427386  push       esi
00427387  mov        esi, ecx
00427389  test       edx, edx
0042738b  je         0x4273aa

// block 0042738d
0042738d  mov        eax, dword ptr [edx + 0x60]
00427390  mov        ecx, eax
00427392  shr        ecx, 2
00427395  or         ecx, eax
00427397  test       cl, 1
0042739a  je         0x4273a3

// block 0042739c
0042739c  mov        eax, 1
004273a1  pop        esi
004273a2  ret        

// block 004273a3
004273a3  test       eax, 0x400
004273a8  jne        0x42739c

// block 004273aa
004273aa  push       esi
004273ab  call       0x425c20

// block 004273b0
004273b0  pop        esi
004273b1  ret        
