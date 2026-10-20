
// block 0047340e
0047340e  mov        edi, edi
00473410  push       ebp
00473411  mov        ebp, esp
00473413  cmp        dword ptr [0x4b40dc], 0
0047341a  push       1
0047341c  push       dword ptr [ebp + 0x10]
0047341f  push       dword ptr [ebp + 0xc]
00473422  push       dword ptr [ebp + 8]
00473425  jne        0x47342e

// block 00473427
00473427  push       0x4adac0
0047342c  jmp        0x473430

// block 0047342e
0047342e  push       0

// block 00473430
00473430  call       0x473197

// block 00473435
00473435  add        esp, 0x14
00473438  pop        ebp
00473439  ret        
