
// block 00479ff3
00479ff3  mov        edi, edi
00479ff5  push       ebp
00479ff6  mov        ebp, esp
00479ff8  sub        esp, 0x328
00479ffe  mov        eax, dword ptr [0x4ad138]
0047a003  xor        eax, ebp
0047a005  mov        dword ptr [ebp - 4], eax
0047a008  test       byte ptr [0x4adba0], 1
0047a00f  push       esi
0047a010  je         0x47a01a

// block 0047a012
0047a012  push       0xa
0047a014  call       0x472f8d

// block 0047a019
0047a019  pop        ecx

// block 0047a01a
0047a01a  call       0x4829fd

// block 0047a01f
0047a01f  test       eax, eax
0047a021  je         0x47a02b

// block 0047a023
0047a023  push       0x16
0047a025  call       0x482c59

// block 0047a02a
0047a02a  pop        ecx

// block 0047a02b
0047a02b  test       byte ptr [0x4adba0], 2
0047a032  je         0x47a102

// block 0047a038
0047a038  mov        dword ptr [ebp - 0x220], eax
0047a03e  mov        dword ptr [ebp - 0x224], ecx
0047a044  mov        dword ptr [ebp - 0x228], edx
0047a04a  mov        dword ptr [ebp - 0x22c], ebx
0047a050  mov        dword ptr [ebp - 0x230], esi
0047a056  mov        dword ptr [ebp - 0x234], edi
0047a05c  mov        word ptr [ebp - 0x208], ss
0047a063  mov        word ptr [ebp - 0x214], cs
0047a06a  mov        word ptr [ebp - 0x238], ds
0047a071  mov        word ptr [ebp - 0x23c], es
0047a078  mov        word ptr [ebp - 0x240], fs
0047a07f  mov        word ptr [ebp - 0x244], gs
0047a086  pushfd     
0047a087  pop        dword ptr [ebp - 0x210]
0047a08d  mov        esi, dword ptr [ebp + 4]
0047a090  lea        eax, [ebp + 4]
0047a093  mov        dword ptr [ebp - 0x20c], eax
0047a099  mov        dword ptr [ebp - 0x2d0], 0x10001
0047a0a3  mov        dword ptr [ebp - 0x218], esi
0047a0a9  mov        eax, dword ptr [eax - 4]
0047a0ac  push       0x50
0047a0ae  mov        dword ptr [ebp - 0x21c], eax
0047a0b4  lea        eax, [ebp - 0x328]
0047a0ba  push       0
0047a0bc  push       eax
0047a0bd  call       0x477420

// block 0047a0c2
0047a0c2  lea        eax, [ebp - 0x328]
0047a0c8  add        esp, 0xc
0047a0cb  mov        dword ptr [ebp - 0x2d8], eax
0047a0d1  lea        eax, [ebp - 0x2d0]
0047a0d7  push       0
0047a0d9  mov        dword ptr [ebp - 0x328], 0x40000015
0047a0e3  mov        dword ptr [ebp - 0x31c], esi
0047a0e9  mov        dword ptr [ebp - 0x2d4], eax
0047a0ef  call       dword ptr [0x4981c8]

// block 0047a0f5
0047a0f5  lea        eax, [ebp - 0x2d8]
0047a0fb  push       eax
0047a0fc  call       dword ptr [0x4981cc]

// block 0047a102
0047a102  push       3
0047a104  call       0x472f0b

// block 0047a109
0047a109  int3       
