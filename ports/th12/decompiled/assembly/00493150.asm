
// block 00493150
00493150  push       ebp
00493151  mov        ebp, esp
00493153  sub        esp, 4
00493156  push       ebx
00493157  push       ecx
00493158  mov        eax, dword ptr [ebp + 0xc]
0049315b  add        eax, 0xc
0049315e  mov        dword ptr [ebp - 4], eax
00493161  mov        eax, dword ptr [ebp + 8]
00493164  push       ebp
00493165  push       dword ptr [ebp + 0x10]
00493168  mov        ecx, dword ptr [ebp + 0x10]
0049316b  mov        ebp, dword ptr [ebp - 4]
0049316e  call       0x48c994

// block 00493173
00493173  push       esi
00493174  push       edi
00493175  call       eax

// block 00493177
00493177  pop        edi
00493178  pop        esi
00493179  mov        ebx, ebp
0049317b  pop        ebp
0049317c  mov        ecx, dword ptr [ebp + 0x10]
0049317f  push       ebp
00493180  mov        ebp, ebx
00493182  cmp        ecx, 0x100
00493188  jne        0x49318f

// block 0049318a
0049318a  mov        ecx, 2

// block 0049318f
0049318f  push       ecx
00493190  call       0x48c994

// block 00493195
00493195  pop        ebp
00493196  pop        ecx
00493197  pop        ebx
00493198  leave      
00493199  ret        0xc
