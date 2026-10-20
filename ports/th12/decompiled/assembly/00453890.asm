
// block 00453890
00453890  cmp        dword ptr [0x4d4754], 0
00453897  jne        0x45389d

// block 00453899
00453899  or         eax, 0xffffffff
0045389c  ret        

// block 0045389d
0045389d  push       esi
0045389e  push       0x4cf4e8
004538a3  call       0x453670

// block 004538a8
004538a8  mov        ecx, dword ptr [0x4d4754]
004538ae  mov        ecx, dword ptr [ecx + 0xc]
004538b1  mov        esi, eax
004538b3  imul       eax, eax, 0x34
004538b6  add        eax, dword ptr [0x4d0e6c]
004538bc  push       0
004538be  call       0x466bc0

// block 004538c3
004538c3  push       esi
004538c4  push       0x4a2ef4
004538c9  call       0x454b00

// block 004538ce
004538ce  add        esp, 8
004538d1  xor        eax, eax
004538d3  pop        esi
004538d4  ret        
