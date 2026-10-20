
// block 00473736
00473736  mov        edi, edi
00473738  push       ebp
00473739  mov        ebp, esp
0047373b  cmp        dword ptr [0x4b40dc], 0
00473742  push       1
00473744  push       dword ptr [ebp + 0x10]
00473747  push       dword ptr [ebp + 0xc]
0047374a  push       dword ptr [ebp + 8]
0047374d  jne        0x473756

// block 0047374f
0047374f  push       0x4adac0
00473754  jmp        0x473758

// block 00473756
00473756  push       0

// block 00473758
00473758  call       0x473457

// block 0047375d
0047375d  add        esp, 0x14
00473760  pop        ebp
00473761  ret        
