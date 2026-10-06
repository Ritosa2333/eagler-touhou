
// block 0049205b
0049205b  push       0x10
0049205d  push       0x4ab3d0
00492062  call       0x46fcec

// block 00492067
00492067  mov        edi, dword ptr [ebp + 0x10]
0049206a  mov        ebx, dword ptr [ebp + 8]
0049206d  cmp        dword ptr [edi + 4], 0x80
00492074  jg         0x49207c

// block 00492076
00492076  movsx      esi, byte ptr [ebx + 8]
0049207a  jmp        0x49207f

// block 0049207c
0049207c  mov        esi, dword ptr [ebx + 8]

// block 0049207f
0049207f  mov        dword ptr [ebp - 0x1c], esi
00492082  call       0x475467

// block 00492087
00492087  add        eax, 0x90
0049208c  inc        dword ptr [eax]
0049208e  and        dword ptr [ebp - 4], 0

// block 00492092
00492092  cmp        esi, dword ptr [ebp + 0x14]
00492095  je         0x4920fc

// block 00492097
00492097  cmp        esi, -1
0049209a  jle        0x4920a1

// block 0049209c
0049209c  cmp        esi, dword ptr [edi + 4]
0049209f  jl         0x4920a6

// block 004920a1
004920a1  call       0x472b94

// block 004920a6
004920a6  mov        eax, esi
004920a8  shl        eax, 3
004920ab  mov        ecx, dword ptr [edi + 8]
004920ae  add        ecx, eax
004920b0  mov        esi, dword ptr [ecx]
004920b2  mov        dword ptr [ebp - 0x20], esi
004920b5  mov        dword ptr [ebp - 4], 1
004920bc  cmp        dword ptr [ecx + 4], 0
004920c0  je         0x4920d7

// block 004920c2
004920c2  mov        dword ptr [ebx + 8], esi
004920c5  push       0x103
004920ca  push       ebx
004920cb  mov        ecx, dword ptr [edi + 8]
004920ce  push       dword ptr [ecx + eax + 4]
004920d2  call       0x493150

// block 004920d7
004920d7  and        dword ptr [ebp - 4], 0
004920db  jmp        0x4920f7

// block 004920f7
004920f7  mov        dword ptr [ebp - 0x1c], esi
004920fa  jmp        0x492092

// block 004920fc
004920fc  mov        dword ptr [ebp - 4], 0xfffffffe
00492103  call       0x492121

// block 00492108
00492108  cmp        esi, dword ptr [ebp + 0x14]
0049210b  je         0x492112

// block 0049210d
0049210d  call       0x472b94

// block 00492112
00492112  mov        dword ptr [ebx + 8], esi
00492115  call       0x46fd31

// block 0049211a
0049211a  ret        
