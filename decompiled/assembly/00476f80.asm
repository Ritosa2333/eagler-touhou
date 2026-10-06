
// block 00476f80
00476f80  push       8
00476f82  push       0x4aad98
00476f87  call       0x46fcec

// block 00476f8c
00476f8c  xor        esi, esi
00476f8e  cmp        dword ptr [0x4b41cc], esi
00476f94  jne        0x476fc0

// block 00476f96
00476f96  push       6
00476f98  call       0x46ec9a

// block 00476f9d
00476f9d  pop        ecx
00476f9e  mov        dword ptr [ebp - 4], esi
00476fa1  cmp        dword ptr [0x4b41cc], esi
00476fa7  jne        0x476fb4

// block 00476fa9
00476fa9  call       0x47686b

// block 00476fae
00476fae  inc        dword ptr [0x4b41cc]

// block 00476fb4
00476fb4  mov        dword ptr [ebp - 4], 0xfffffffe
00476fbb  call       0x476fc6

// block 00476fc0
00476fc0  call       0x46fd31

// block 00476fc5
00476fc5  ret        
