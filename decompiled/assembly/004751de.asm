
// block 004751de
004751de  mov        edi, edi
004751e0  push       ebp
004751e1  mov        ebp, esp
004751e3  push       esi
004751e4  push       dword ptr [0x4adacc]
004751ea  mov        esi, dword ptr [0x49814c]
004751f0  call       esi

// block 004751f2
004751f2  test       eax, eax
004751f4  je         0x475217

// block 004751f6
004751f6  mov        eax, dword ptr [0x4adac8]
004751fb  cmp        eax, -1
004751fe  je         0x475217

// block 00475200
00475200  push       eax
00475201  push       dword ptr [0x4adacc]
00475207  call       esi

// block 00475209
00475209  call       eax

// block 0047520b
0047520b  test       eax, eax
0047520d  je         0x475217

// block 0047520f
0047520f  mov        eax, dword ptr [eax + 0x1fc]
00475215  jmp        0x47523e

// block 00475217
00475217  mov        esi, 0x49d51c
0047521c  push       esi
0047521d  call       dword ptr [0x498174]

// block 00475223
00475223  test       eax, eax
00475225  jne        0x475232

// block 00475227
00475227  push       esi
00475228  call       0x472bdd

// block 0047522d
0047522d  pop        ecx
0047522e  test       eax, eax
00475230  je         0x47524a

// block 00475232
00475232  push       0x49d538
00475237  push       eax
00475238  call       dword ptr [0x498170]

// block 0047523e
0047523e  test       eax, eax
00475240  je         0x47524a

// block 00475242
00475242  push       dword ptr [ebp + 8]
00475245  call       eax

// block 00475247
00475247  mov        dword ptr [ebp + 8], eax

// block 0047524a
0047524a  mov        eax, dword ptr [ebp + 8]
0047524d  pop        esi
0047524e  pop        ebp
0047524f  ret        
