
// block 00454ee0
00454ee0  push       edi
00454ee1  mov        edi, dword ptr [ecx + 0x11c]
00454ee7  cmp        dword ptr [edi + edx*4], 0
00454eeb  je         0x454f87

// block 00454ef1
00454ef1  cmp        dword ptr [ecx + 0x124], 0
00454ef8  jne        0x454f87

// block 00454efe
00454efe  fldz       
00454f00  mov        word ptr [eax + 0x3ea], dx
00454f07  mov        di, word ptr [ecx]
00454f0a  and        dword ptr [eax + 0x47c], 0xfffff3ff
00454f14  mov        word ptr [eax + 0x3e6], di
00454f1b  mov        dword ptr [eax + 0x3f8], ecx
00454f21  mov        ecx, dword ptr [ecx + 0x11c]
00454f27  mov        ecx, dword ptr [ecx + edx*4]
00454f2a  mov        dword ptr [eax + 0x3ec], ecx
00454f30  mov        dword ptr [eax + 0x3f0], ecx
00454f36  mov        ecx, dword ptr [eax + 0x78]
00454f39  test       cl, 1
00454f3c  jne        0x454f5c

// block 00454f3e
00454f3e  or         ecx, 1
00454f41  fst        dword ptr [eax + 0x70]
00454f44  mov        dword ptr [eax + 0x6c], 0
00454f4b  mov        dword ptr [eax + 0x68], 0xfff0bdc1
00454f52  mov        dword ptr [eax + 0x74], 0x4b2ed0
00454f59  mov        dword ptr [eax + 0x78], ecx

// block 00454f5c
00454f5c  fstp       dword ptr [eax + 0x70]
00454f5f  mov        dword ptr [eax + 0x6c], 0
00454f66  mov        dword ptr [eax + 0x68], 0xffffffff
00454f6d  and        dword ptr [eax + 0x47c], 0xfffffffe
00454f74  push       eax
00454f75  call       0x455630

// block 00454f7a
00454f7a  mov        eax, dword ptr [0x4ce8cc]
00454f7f  inc        dword ptr [eax + 0xa0]
00454f85  pop        edi
00454f86  ret        

// block 00454f87
00454f87  push       0x4b4
00454f8c  push       0
00454f8e  push       eax
00454f8f  call       0x477420

// block 00454f94
00454f94  add        esp, 0xc
00454f97  pop        edi
00454f98  ret        
