
// block 0043ab60
0043ab60  push       ebp
0043ab61  mov        ebp, esp
0043ab63  and        esp, 0xfffffff8
0043ab66  push       ecx
0043ab67  push       esi
0043ab68  mov        esi, edx
0043ab6a  cmp        dword ptr [esi + 0x48], 2
0043ab6e  je         0x43abe6

// block 0043ab70
0043ab70  mov        eax, dword ptr [0x4b43c4]
0043ab75  fld        dword ptr [esi + 0x24]
0043ab78  cmp        dword ptr [eax + 0x3c], 0
0043ab7c  je         0x43ab86

// block 0043ab7e
0043ab7e  fsub       qword ptr [0x4a40a8]
0043ab84  jmp        0x43ab8c

// block 0043ab86
0043ab86  fsub       qword ptr [0x4a40a0]

// block 0043ab8c
0043ab8c  fstp       dword ptr [esi + 0x24]
0043ab8f  fld        dword ptr [esi + 0x70]
0043ab92  fadd       dword ptr [esi + 0x20]
0043ab95  fstp       dword ptr [esi + 0x20]
0043ab98  fld        dword ptr [esi + 0x24]
0043ab9b  fld        dword ptr [esi + 0x20]
0043ab9e  call       0x4937aa

// block 0043aba3
0043aba3  fstp       dword ptr [esp + 4]
0043aba7  fld        dword ptr [esp + 4]
0043abab  push       ecx
0043abac  fstp       dword ptr [esp]
0043abaf  call       0x4646e0

// block 0043abb4
0043abb4  push       ecx
0043abb5  fstp       dword ptr [esp]
0043abb8  call       0x4646e0

// block 0043abbd
0043abbd  fstp       dword ptr [esi + 0x30]
0043abc0  fld        dword ptr [esi + 0x24]
0043abc3  fld        dword ptr [esi + 0x20]
0043abc6  fmul       st(0), st(0)
0043abc8  fld        st(1)
0043abca  fmulp      st(2)
0043abcc  faddp      st(1)
0043abce  fstp       dword ptr [esp + 4]
0043abd2  fld        dword ptr [esp + 4]
0043abd6  call       0x4937c0

// block 0043abdb
0043abdb  fstp       dword ptr [esp + 4]
0043abdf  fld        dword ptr [esp + 4]
0043abe3  fstp       dword ptr [esi + 0x2c]

// block 0043abe6
0043abe6  xor        eax, eax
0043abe8  pop        esi
0043abe9  mov        esp, ebp
0043abeb  pop        ebp
0043abec  ret        
