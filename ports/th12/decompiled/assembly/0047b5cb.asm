
// block 0047b5cb
0047b5cb  mov        edi, edi
0047b5cd  push       ebp
0047b5ce  mov        ebp, esp
0047b5d0  push       ecx
0047b5d1  push       ecx
0047b5d2  mov        eax, dword ptr [ebp + 0xc]
0047b5d5  push       esi
0047b5d6  mov        esi, dword ptr [ebp + 8]
0047b5d9  mov        dword ptr [ebp - 8], eax
0047b5dc  mov        eax, dword ptr [ebp + 0x10]
0047b5df  push       edi
0047b5e0  push       esi
0047b5e1  mov        dword ptr [ebp - 4], eax
0047b5e4  call       0x48cac6

// block 0047b5e9
0047b5e9  or         edi, 0xffffffff
0047b5ec  pop        ecx
0047b5ed  cmp        eax, edi
0047b5ef  jne        0x47b602

// block 0047b5f1
0047b5f1  call       0x46e96f

// block 0047b5f6
0047b5f6  mov        dword ptr [eax], 9

// block 0047b5fc
0047b5fc  mov        eax, edi
0047b5fe  mov        edx, edi
0047b600  jmp        0x47b64c

// block 0047b602
0047b602  push       dword ptr [ebp + 0x14]
0047b605  lea        ecx, [ebp - 4]
0047b608  push       ecx
0047b609  push       dword ptr [ebp - 8]
0047b60c  push       eax
0047b60d  call       dword ptr [0x4980a0]

// block 0047b613
0047b613  mov        dword ptr [ebp - 8], eax
0047b616  cmp        eax, edi
0047b618  jne        0x47b62d

// block 0047b61a
0047b61a  call       dword ptr [0x4980e4]

// block 0047b620
0047b620  test       eax, eax
0047b622  je         0x47b62d

// block 0047b624
0047b624  push       eax
0047b625  call       0x46e995

// block 0047b62a
0047b62a  pop        ecx
0047b62b  jmp        0x47b5fc

// block 0047b62d
0047b62d  mov        eax, esi
0047b62f  sar        eax, 5
0047b632  mov        eax, dword ptr [eax*4 + 0x4d6320]
0047b639  and        esi, 0x1f
0047b63c  shl        esi, 6
0047b63f  lea        eax, [eax + esi + 4]
0047b643  and        byte ptr [eax], 0xfd
0047b646  mov        eax, dword ptr [ebp - 8]
0047b649  mov        edx, dword ptr [ebp - 4]

// block 0047b64c
0047b64c  pop        edi
0047b64d  pop        esi
0047b64e  leave      
0047b64f  ret        
