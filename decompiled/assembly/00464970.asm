
// block 00464970
00464970  push       esi
00464971  mov        esi, dword ptr [eax + 8]
00464974  test       esi, esi
00464976  jle        0x4649e3

// block 00464978
00464978  push       ebx
00464979  push       ebp
0046497a  push       edi
0046497b  mov        edi, dword ptr [eax + 0xd4]
00464981  lea        ebx, [eax + 0x90]

// block 00464987
00464987  mov        ecx, dword ptr [esp + 0x14]
0046498b  add        dword ptr [eax], ecx
0046498d  cmp        dword ptr [eax], esi
0046498f  jl         0x4649aa

// block 00464991
00464991  mov        ecx, dword ptr [eax + 0xd0]

// block 00464997
00464997  test       ecx, ecx
00464999  je         0x46499f

// block 0046499b
0046499b  sub        dword ptr [eax], esi
0046499d  jmp        0x4649a4

// block 0046499f
0046499f  lea        edx, [esi - 1]
004649a2  mov        dword ptr [eax], edx

// block 004649a4
004649a4  mov        edx, dword ptr [eax]
004649a6  cmp        edx, esi
004649a8  jge        0x464997

// block 004649aa
004649aa  cmp        dword ptr [eax], 0
004649ad  jge        0x4649c8

// block 004649af
004649af  mov        ecx, dword ptr [eax + 0xd0]

// block 004649b5
004649b5  test       ecx, ecx
004649b7  je         0x4649bd

// block 004649b9
004649b9  add        dword ptr [eax], esi
004649bb  jmp        0x4649c3

// block 004649bd
004649bd  mov        dword ptr [eax], 0

// block 004649c3
004649c3  cmp        dword ptr [eax], 0
004649c6  jl         0x4649b5

// block 004649c8
004649c8  xor        ecx, ecx
004649ca  mov        edx, ebx
004649cc  lea        esp, [esp]

// block 004649d0
004649d0  cmp        ecx, edi
004649d2  jge        0x4649e0

// block 004649d4
004649d4  mov        ebp, dword ptr [edx]
004649d6  cmp        ebp, dword ptr [eax]
004649d8  je         0x464987

// block 004649da
004649da  inc        ecx
004649db  add        edx, 4
004649de  jmp        0x4649d0

// block 004649e0
004649e0  pop        edi
004649e1  pop        ebp
004649e2  pop        ebx

// block 004649e3
004649e3  mov        eax, dword ptr [eax]
004649e5  pop        esi
004649e6  ret        4
