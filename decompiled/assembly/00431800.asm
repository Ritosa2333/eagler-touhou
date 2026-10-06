
// block 00431800
00431800  test       dword ptr [edi + 0x590], 0x8000
0043180a  je         0x431824

// block 0043180c
0043180c  lea        eax, [esi + esi*2 + 0x102]
00431813  lea        ecx, [edi + eax*8]
00431816  push       ecx
00431817  call       dword ptr [0x49808c]

// block 0043181d
0043181d  dec        byte ptr [esi + edi + 0x930]

// block 00431824
00431824  ret        
