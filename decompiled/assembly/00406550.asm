
// block 00406550
00406550  mov        eax, ebx
00406552  imul       eax, eax, 0x118
00406558  push       edi
00406559  lea        edi, [eax + esi + 0x204]
00406560  mov        dword ptr [esi + 0x54c], edi
00406566  call       0x430910

// block 0040656b
0040656b  mov        edx, dword ptr [esi + 0x54c]
00406571  mov        eax, dword ptr [esi + 8]
00406574  mov        ecx, dword ptr [eax]
00406576  add        edx, 0xcc
0040657c  push       edx
0040657d  push       eax
0040657e  mov        eax, dword ptr [ecx + 0xbc]
00406584  call       eax

// block 00406586
00406586  mov        dword ptr [esi + 0x550], ebx
0040658c  pop        edi
0040658d  ret        
