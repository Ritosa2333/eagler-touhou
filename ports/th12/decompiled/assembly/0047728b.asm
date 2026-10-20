
// block 0047728b
0047728b  mov        edi, edi
0047728d  push       ebp
0047728e  mov        ebp, esp
00477290  push       esi
00477291  call       0x477735

// block 00477296
00477296  mov        esi, eax
00477298  test       esi, esi
0047729a  je         0x4772af

// block 0047729c
0047729c  push       dword ptr [ebp + 8]
0047729f  push       esi
004772a0  call       0x477048

// block 004772a5
004772a5  neg        eax
004772a7  sbb        eax, eax
004772a9  pop        ecx
004772aa  not        eax
004772ac  pop        ecx
004772ad  and        eax, esi

// block 004772af
004772af  pop        esi
004772b0  pop        ebp
004772b1  ret        
