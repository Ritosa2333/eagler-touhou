
// block 00402030
00402030  push       esi
00402031  mov        esi, dword ptr [0x4ce8cc]
00402037  push       edi
00402038  call       0x45a3c0

// block 0040203d
0040203d  mov        eax, dword ptr [esp + 0xc]
00402041  push       1
00402043  push       eax
00402044  call       0x401790

// block 00402049
00402049  mov        esi, dword ptr [0x4ce8cc]
0040204f  call       0x45a3c0

// block 00402054
00402054  mov        edi, 0x4cec04
00402059  mov        dword ptr [0x4cee34], edi
0040205f  call       0x430a70

// block 00402064
00402064  mov        edx, dword ptr [0x4cee34]
0040206a  mov        eax, dword ptr [0x4ce8f0]
0040206f  mov        ecx, dword ptr [eax]
00402071  add        edx, 0xcc
00402077  push       edx
00402078  push       eax
00402079  mov        eax, dword ptr [ecx + 0xbc]
0040207f  call       eax

// block 00402081
00402081  pop        edi
00402082  mov        dword ptr [0x4cee38], 1
0040208c  mov        eax, 1
00402091  pop        esi
00402092  ret        4
