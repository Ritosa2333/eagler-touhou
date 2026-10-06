
// block 0046eb51
0046eb51  mov        edi, edi

// block 004752ca
004752ca  mov        eax, dword ptr [0x4adac8]
004752cf  cmp        eax, -1
004752d2  je         0x4752ea

// block 004752d4
004752d4  push       eax
004752d5  push       dword ptr [0x4b410c]
004752db  call       0x4751de

// block 004752e0
004752e0  pop        ecx
004752e1  call       eax

// block 004752e3
004752e3  or         dword ptr [0x4adac8], 0xffffffff

// block 004752ea
004752ea  mov        eax, dword ptr [0x4adacc]
004752ef  cmp        eax, -1
004752f2  je         0x475302

// block 004752f4
004752f4  push       eax
004752f5  call       dword ptr [0x498140]

// block 004752fb
004752fb  or         dword ptr [0x4adacc], 0xffffffff

// block 00475302
00475302  jmp        0x46eb51
