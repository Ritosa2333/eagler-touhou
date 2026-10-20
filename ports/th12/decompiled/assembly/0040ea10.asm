
// block 0040ea10
0040ea10  push       ecx
0040ea11  mov        eax, dword ptr [0x4b4514]
0040ea16  mov        ecx, dword ptr [eax + 8]
0040ea19  mov        edx, 2
0040ea1e  or         dword ptr [ecx + 4], edx
0040ea21  mov        ecx, dword ptr [eax + 0xc]
0040ea24  or         dword ptr [ecx + 4], edx
0040ea27  push       eax
0040ea28  call       0x4385b0

// block 0040ea2d
0040ea2d  pop        ecx
0040ea2e  ret        
