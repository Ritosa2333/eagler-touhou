
// block 0044edf0
0044edf0  cmp        dword ptr [ecx + 0x18], 0x10
0044edf4  mov        eax, dword ptr [esp + 4]
0044edf8  mov        dword ptr [ecx + 0x14], eax
0044edfb  jb         0x44ee07

// block 0044edfd
0044edfd  mov        ecx, dword ptr [ecx + 4]
0044ee00  mov        byte ptr [ecx + eax], 0
0044ee04  ret        4

// block 0044ee07
0044ee07  mov        byte ptr [ecx + eax + 4], 0
0044ee0c  ret        4
