
// block 004916e6
004916e6  push       0x44
004916e8  mov        eax, 0x496d4e
004916ed  call       0x491e59

// block 004916f2
004916f2  push       0x49f3c0
004916f7  lea        ecx, [ebp - 0x28]
004916fa  call       0x4916bf

// block 004916ff
004916ff  and        dword ptr [ebp - 4], 0
00491703  lea        eax, [ebp - 0x28]
00491706  push       eax
00491707  lea        ecx, [ebp - 0x50]
0049170a  call       0x491638

// block 0049170f
0049170f  push       0x4ab2d4
00491714  lea        eax, [ebp - 0x50]
00491717  push       eax
00491718  call       0x46ff8f

// block 0049171d
0049171d  int3       

// block 00496d46
00496d46  lea        ecx, [ebp - 0x28]
00496d49  jmp        0x44ea80

// block 00496d4e
00496d4e  mov        edx, dword ptr [esp + 8]
00496d52  lea        eax, [edx + 0xc]
00496d55  mov        ecx, dword ptr [edx - 0x54]
00496d58  xor        ecx, eax
00496d5a  call       0x46c932

// block 00496d5f
00496d5f  mov        eax, 0x4ab2ec
00496d64  jmp        0x491aab
