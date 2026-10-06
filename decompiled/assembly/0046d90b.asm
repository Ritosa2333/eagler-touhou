
// block 0046d90b
0046d90b  mov        edi, edi
0046d90d  push       ebp
0046d90e  mov        ebp, esp
0046d910  push       ecx
0046d911  push       ebx
0046d912  push       esi
0046d913  push       edi
0046d914  push       dword ptr [0x4d6430]
0046d91a  call       0x4751de

// block 0046d91f
0046d91f  push       dword ptr [0x4d642c]
0046d925  mov        edi, eax
0046d927  mov        dword ptr [ebp - 4], edi
0046d92a  call       0x4751de

// block 0046d92f
0046d92f  mov        esi, eax
0046d931  pop        ecx
0046d932  pop        ecx
0046d933  cmp        esi, edi
0046d935  jb         0x46d9be

// block 0046d93b
0046d93b  mov        ebx, esi
0046d93d  sub        ebx, edi
0046d93f  lea        eax, [ebx + 4]
0046d942  cmp        eax, 4
0046d945  jb         0x46d9be

// block 0046d947
0046d947  push       edi
0046d948  call       0x4778ff

// block 0046d94d
0046d94d  mov        edi, eax
0046d94f  lea        eax, [ebx + 4]
0046d952  pop        ecx
0046d953  cmp        edi, eax
0046d955  jae        0x46d99f

// block 0046d957
0046d957  mov        eax, 0x800
0046d95c  cmp        edi, eax
0046d95e  jae        0x46d962

// block 0046d960
0046d960  mov        eax, edi

// block 0046d962
0046d962  add        eax, edi
0046d964  cmp        eax, edi
0046d966  jb         0x46d977

// block 0046d968
0046d968  push       eax
0046d969  push       dword ptr [ebp - 4]
0046d96c  call       0x47785f

// block 0046d971
0046d971  pop        ecx
0046d972  pop        ecx
0046d973  test       eax, eax
0046d975  jne        0x46d98d

// block 0046d977
0046d977  lea        eax, [edi + 0x10]
0046d97a  cmp        eax, edi
0046d97c  jb         0x46d9be

// block 0046d97e
0046d97e  push       eax
0046d97f  push       dword ptr [ebp - 4]
0046d982  call       0x47785f

// block 0046d987
0046d987  pop        ecx
0046d988  pop        ecx
0046d989  test       eax, eax
0046d98b  je         0x46d9be

// block 0046d98d
0046d98d  sar        ebx, 2
0046d990  push       eax
0046d991  lea        esi, [eax + ebx*4]
0046d994  call       0x475163

// block 0046d999
0046d999  pop        ecx
0046d99a  mov        dword ptr [0x4d6430], eax

// block 0046d99f
0046d99f  push       dword ptr [ebp + 8]
0046d9a2  call       0x475163

// block 0046d9a7
0046d9a7  mov        dword ptr [esi], eax
0046d9a9  add        esi, 4
0046d9ac  push       esi
0046d9ad  call       0x475163

// block 0046d9b2
0046d9b2  pop        ecx
0046d9b3  mov        dword ptr [0x4d642c], eax
0046d9b8  mov        eax, dword ptr [ebp + 8]
0046d9bb  pop        ecx
0046d9bc  jmp        0x46d9c0

// block 0046d9be
0046d9be  xor        eax, eax

// block 0046d9c0
0046d9c0  pop        edi
0046d9c1  pop        esi
0046d9c2  pop        ebx
0046d9c3  leave      
0046d9c4  ret        
