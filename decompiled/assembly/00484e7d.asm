
// block 00484e7d
00484e7d  cmp        dword ptr [eax], 0
00484e80  push       esi
00484e81  mov        esi, edx
00484e83  je         0x484e9b

// block 00484e85
00484e85  push       edi

// block 00484e86
00484e86  mov        dl, byte ptr [esi]
00484e88  test       dl, dl
00484e8a  je         0x484e9a

// block 00484e8c
00484e8c  mov        edi, dword ptr [ecx]
00484e8e  mov        byte ptr [edi], dl
00484e90  inc        dword ptr [ecx]
00484e92  inc        esi
00484e93  dec        dword ptr [eax]
00484e95  cmp        dword ptr [eax], 0
00484e98  jne        0x484e86

// block 00484e9a
00484e9a  pop        edi

// block 00484e9b
00484e9b  pop        esi
00484e9c  ret        
