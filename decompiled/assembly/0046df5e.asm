
// block 0046df5e
0046df5e  mov        edi, edi
0046df60  push       ebp
0046df61  mov        ebp, esp
0046df63  push       ebx
0046df64  mov        ebx, dword ptr [ebp + 8]
0046df67  push       esi
0046df68  push       edi
0046df69  mov        edi, ecx
0046df6b  mov        dword ptr [edi], 0x49cd28
0046df71  mov        eax, dword ptr [ebx]
0046df73  test       eax, eax
0046df75  je         0x46df9d

// block 0046df77
0046df77  push       eax
0046df78  call       0x475860

// block 0046df7d
0046df7d  mov        esi, eax
0046df7f  inc        esi
0046df80  push       esi
0046df81  call       0x46d04a

// block 0046df86
0046df86  pop        ecx
0046df87  pop        ecx
0046df88  mov        dword ptr [edi + 4], eax
0046df8b  test       eax, eax
0046df8d  je         0x46dfa1

// block 0046df8f
0046df8f  push       dword ptr [ebx]
0046df91  push       esi
0046df92  push       eax
0046df93  call       0x47a2b9

// block 0046df98
0046df98  add        esp, 0xc
0046df9b  jmp        0x46dfa1

// block 0046df9d
0046df9d  and        dword ptr [edi + 4], 0

// block 0046dfa1
0046dfa1  mov        dword ptr [edi + 8], 1
0046dfa8  mov        eax, edi
0046dfaa  pop        edi
0046dfab  pop        esi
0046dfac  pop        ebx
0046dfad  pop        ebp
0046dfae  ret        4
