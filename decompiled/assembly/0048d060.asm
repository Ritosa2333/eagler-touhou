
// block 0048d060
0048d060  push       ecx
0048d061  lea        ecx, [esp + 4]
0048d065  sub        ecx, eax
0048d067  sbb        eax, eax
0048d069  not        eax
0048d06b  and        ecx, eax
0048d06d  mov        eax, esp
0048d06f  and        eax, 0xfffff000

// block 0048d074
0048d074  cmp        ecx, eax
0048d076  jb         0x48d082

// block 0048d078
0048d078  mov        eax, ecx
0048d07a  pop        ecx
0048d07b  xchg       esp, eax
0048d07c  mov        eax, dword ptr [eax]
0048d07e  mov        dword ptr [esp], eax
0048d081  ret        

// block 0048d082
0048d082  sub        eax, 0x1000
0048d087  test       dword ptr [eax], eax
0048d089  jmp        0x48d074
