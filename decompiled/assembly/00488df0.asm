
// block 00488df0
00488df0  push       ebp
00488df1  mov        ebp, esp
00488df3  push       esi
00488df4  xor        eax, eax
00488df6  push       eax
00488df7  push       eax
00488df8  push       eax
00488df9  push       eax
00488dfa  push       eax
00488dfb  push       eax
00488dfc  push       eax
00488dfd  push       eax
00488dfe  mov        edx, dword ptr [ebp + 0xc]
00488e01  lea        ecx, [ecx]

// block 00488e04
00488e04  mov        al, byte ptr [edx]
00488e06  or         al, al
00488e08  je         0x488e13

// block 00488e0a
00488e0a  add        edx, 1
00488e0d  bts        dword ptr [esp], eax
00488e11  jmp        0x488e04

// block 00488e13
00488e13  mov        esi, dword ptr [ebp + 8]
00488e16  mov        edi, edi

// block 00488e18
00488e18  mov        al, byte ptr [esi]
00488e1a  or         al, al
00488e1c  je         0x488e2a

// block 00488e1e
00488e1e  add        esi, 1
00488e21  bt         dword ptr [esp], eax
00488e25  jae        0x488e18

// block 00488e27
00488e27  lea        eax, [esi - 1]

// block 00488e2a
00488e2a  add        esp, 0x20
00488e2d  pop        esi
00488e2e  leave      
00488e2f  ret        
