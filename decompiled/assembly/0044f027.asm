
// block 0044f027
0044f027  mov        esi, dword ptr [ebp - 0x14]
0044f02a  cmp        dword ptr [esi + 0x18], 0x10
0044f02e  jb         0x44f03c

// block 0044f030
0044f030  mov        edx, dword ptr [esi + 4]
0044f033  push       edx
0044f034  call       0x46ca4f

// block 0044f039
0044f039  add        esp, 4

// block 0044f03c
0044f03c  push       0
0044f03e  mov        dword ptr [esi + 0x18], 0xf
0044f045  mov        dword ptr [esi + 0x14], 0
0044f04c  push       0
0044f04e  mov        byte ptr [esi + 4], 0
0044f052  call       0x46ff8f

// block 0044f057
0044f057  int3       
