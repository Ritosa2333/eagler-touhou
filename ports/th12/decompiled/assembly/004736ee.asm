
// block 004736ee
004736ee  mov        edi, edi
004736f0  push       ebp
004736f1  mov        ebp, esp
004736f3  xor        eax, eax
004736f5  push       eax
004736f6  push       dword ptr [ebp + 0x10]
004736f9  push       dword ptr [ebp + 0xc]
004736fc  push       dword ptr [ebp + 8]
004736ff  cmp        dword ptr [0x4b40dc], eax
00473705  jne        0x47370e

// block 00473707
00473707  push       0x4adac0
0047370c  jmp        0x47370f

// block 0047370e
0047370e  push       eax

// block 0047370f
0047370f  call       0x473457

// block 00473714
00473714  add        esp, 0x14
00473717  pop        ebp
00473718  ret        
