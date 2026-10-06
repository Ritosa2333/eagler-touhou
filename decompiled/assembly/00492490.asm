
// block 00492490
00492490  push       8
00492492  push       0x4ab438
00492497  call       0x46fcec

// block 0049249c
0049249c  and        dword ptr [ebp - 4], 0
004924a0  push       dword ptr [ebp + 0xc]
004924a3  call       dword ptr [ebp + 8]

// block 004924a6
004924a6  pop        ecx
004924a7  jmp        0x4924b6

// block 004924b6
004924b6  mov        dword ptr [ebp - 4], 0xfffffffe
004924bd  call       0x46fd31

// block 004924c2
004924c2  ret        
