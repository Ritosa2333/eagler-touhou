
// block 0049171e
0049171e  push       0x44
00491720  mov        eax, 0x496d71
00491725  call       0x491e59

// block 0049172a
0049172a  push       0x49f3d8
0049172f  lea        ecx, [ebp - 0x28]
00491732  call       0x4916bf

// block 00491737
00491737  and        dword ptr [ebp - 4], 0
0049173b  lea        eax, [ebp - 0x28]
0049173e  push       eax
0049173f  lea        ecx, [ebp - 0x50]
00491742  call       0x49159a

// block 00491747
00491747  push       0x4ab33c
0049174c  lea        eax, [ebp - 0x50]
0049174f  push       eax
00491750  call       0x46ff8f

// block 00491755
00491755  int3       

// block 00496d69
00496d69  lea        ecx, [ebp - 0x28]
00496d6c  jmp        0x44ea80

// block 00496d71
00496d71  mov        edx, dword ptr [esp + 8]
00496d75  lea        eax, [edx + 0xc]
00496d78  mov        ecx, dword ptr [edx - 0x54]
00496d7b  xor        ecx, eax
00496d7d  call       0x46c932

// block 00496d82
00496d82  mov        eax, 0x4ab354
00496d87  jmp        0x491aab
