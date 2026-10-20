
// block 0044b610
0044b610  push       esi
0044b611  push       edi
0044b612  mov        esi, ecx
0044b614  mov        edi, eax
0044b616  call       0x44b710

// block 0044b61b
0044b61b  push       edi
0044b61c  push       0x4a2178
0044b621  call       0x44bcd0

// block 0044b626
0044b626  push       0xc
0044b628  call       0x46c9ea

// block 0044b62d
0044b62d  add        esp, 0xc
0044b630  test       eax, eax
0044b632  je         0x44b64a

// block 0044b634
0044b634  mov        dword ptr [eax], 0x4a22dc
0044b63a  mov        dword ptr [eax + 4], 0xffffffff
0044b641  mov        dword ptr [eax + 8], 0
0044b648  jmp        0x44b64c

// block 0044b64a
0044b64a  xor        eax, eax

// block 0044b64c
0044b64c  mov        dword ptr [esi + 0xc], eax
0044b64f  test       eax, eax
0044b651  je         0x44b691

// block 0044b653
0044b653  mov        edx, edi
0044b655  call       0x44b960

// block 0044b65a
0044b65a  test       al, al
0044b65c  je         0x44b67e

// block 0044b65e
0044b65e  call       0x44bc20

// block 0044b663
0044b663  mov        dword ptr [esi + 8], eax
0044b666  test       eax, eax
0044b668  je         0x44b67e

// block 0044b66a
0044b66a  mov        ecx, dword ptr [esi + 0xc]
0044b66d  mov        edx, dword ptr [ecx]
0044b66f  push       0x4a2334
0044b674  push       eax
0044b675  mov        eax, dword ptr [edx]
0044b677  call       eax

// block 0044b679
0044b679  pop        edi
0044b67a  mov        al, 1
0044b67c  pop        esi
0044b67d  ret        

// block 0044b67e
0044b67e  push       edi
0044b67f  push       0x4a21b4
0044b684  call       0x44bcd0

// block 0044b689
0044b689  add        esp, 8
0044b68c  call       0x44b710

// block 0044b691
0044b691  pop        edi
0044b692  xor        al, al
0044b694  pop        esi
0044b695  ret        
