
// block 00478dc3
00478dc3  dec        dword ptr [edx + 4]
00478dc6  js         0x478dd1

// block 00478dc8
00478dc8  mov        ecx, dword ptr [edx]
00478dca  movzx      eax, byte ptr [ecx]
00478dcd  inc        ecx
00478dce  mov        dword ptr [edx], ecx
00478dd0  ret        

// block 00478dd1
00478dd1  push       edx
00478dd2  call       0x48bed9

// block 00478dd7
00478dd7  pop        ecx
00478dd8  ret        
