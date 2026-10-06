
// block 0043daf0
0043daf0  lea        ecx, [esi + 0x18]
0043daf3  call       0x4027e0

// block 0043daf8
0043daf8  mov        ecx, 0xc
0043dafd  lea        eax, [esi + 0x4f8]

// block 0043db03
0043db03  and        dword ptr [eax], 0xfffffffe
0043db06  add        eax, 0x40
0043db09  sub        ecx, 1
0043db0c  jns        0x43db03

// block 0043db0e
0043db0e  push       0x80c
0043db13  push       0
0043db15  push       esi
0043db16  call       0x477420

// block 0043db1b
0043db1b  or         dword ptr [esi], 2
0043db1e  add        esp, 0xc
0043db21  mov        dword ptr [0x4b4524], esi
0043db27  mov        eax, esi
0043db29  ret        
