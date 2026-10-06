
// block 00435650
00435650  mov        eax, dword ptr [ecx]
00435652  test       eax, eax
00435654  je         0x43566f

// block 00435656
00435656  cmp        dword ptr [ecx + 0x14], 0
0043565a  je         0x43566f

// block 0043565c
0043565c  mov        ecx, dword ptr [ecx + 8]
0043565f  mov        ecx, dword ptr [ecx + 0xc]
00435662  mov        edx, dword ptr [eax]
00435664  mov        edx, dword ptr [edx + 0x30]
00435667  push       ecx
00435668  push       0
0043566a  push       0
0043566c  push       eax
0043566d  call       edx

// block 0043566f
0043566f  ret        
