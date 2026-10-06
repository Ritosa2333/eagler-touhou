
// block 00492745
00492745  mov        esp, dword ptr [ebp - 0x18]
00492748  call       0x475467

// block 0049274d
0049274d  and        dword ptr [eax + 0x20c], 0
00492754  mov        esi, dword ptr [ebp + 0x14]
00492757  mov        edi, dword ptr [ebp + 0xc]
0049275a  cmp        dword ptr [esi + 4], 0x80
00492761  jg         0x492769

// block 00492763
00492763  movsx      ecx, byte ptr [edi + 8]
00492767  jmp        0x49276c

// block 00492769
00492769  mov        ecx, dword ptr [edi + 8]

// block 0049276c
0049276c  mov        ebx, dword ptr [esi + 0x10]
0049276f  and        dword ptr [ebp - 0x20], 0

// block 00492773
00492773  mov        eax, dword ptr [ebp - 0x20]
00492776  cmp        eax, dword ptr [esi + 0xc]
00492779  jae        0x492793

// block 0049277b
0049277b  imul       eax, eax, 0x14
0049277e  add        eax, ebx
00492780  mov        edx, dword ptr [eax + 4]
00492783  cmp        ecx, edx
00492785  jle        0x4927c7

// block 00492787
00492787  cmp        ecx, dword ptr [eax + 8]
0049278a  jg         0x4927c7

// block 0049278c
0049278c  mov        eax, dword ptr [esi + 8]
0049278f  mov        ecx, dword ptr [eax + edx*8 + 8]

// block 00492793
00492793  push       ecx
00492794  push       esi
00492795  push       0
00492797  push       edi
00492798  call       0x49205b

// block 0049279d
0049279d  add        esp, 0x10
004927a0  and        dword ptr [ebp - 0x1c], 0
004927a4  and        dword ptr [ebp - 4], 0
004927a8  mov        esi, dword ptr [ebp + 8]

// block 004927c7
004927c7  inc        dword ptr [ebp - 0x20]
004927ca  jmp        0x492773
