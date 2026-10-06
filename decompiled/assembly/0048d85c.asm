
// block 0048d85c
0048d85c  mov        edi, edi
0048d85e  push       ebp
0048d85f  mov        ebp, esp
0048d861  sub        esp, 0x10
0048d864  push       ebx
0048d865  xor        ebx, ebx
0048d867  push       esi
0048d868  push       edi
0048d869  cmp        dword ptr [ebp + 0x10], ebx
0048d86c  je         0x48d947

// block 0048d872
0048d872  push       dword ptr [ebp + 0x14]
0048d875  lea        ecx, [ebp - 0x10]
0048d878  call       0x46d3b4

// block 0048d87d
0048d87d  cmp        dword ptr [ebp + 8], ebx
0048d880  jne        0x48d8b0

// block 0048d882
0048d882  call       0x46e96f

// block 0048d887
0048d887  push       ebx
0048d888  push       ebx
0048d889  push       ebx
0048d88a  push       ebx
0048d88b  push       ebx
0048d88c  mov        dword ptr [eax], 0x16
0048d892  call       0x470f2d

// block 0048d897
0048d897  add        esp, 0x14
0048d89a  cmp        byte ptr [ebp - 4], bl
0048d89d  je         0x48d8a6

// block 0048d89f
0048d89f  mov        eax, dword ptr [ebp - 8]
0048d8a2  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048d8a6
0048d8a6  mov        eax, 0x7fffffff
0048d8ab  jmp        0x48d949

// block 0048d8b0
0048d8b0  mov        edi, dword ptr [ebp + 0xc]
0048d8b3  cmp        edi, ebx
0048d8b5  je         0x48d882

// block 0048d8b7
0048d8b7  mov        esi, 0x7fffffff
0048d8bc  cmp        dword ptr [ebp + 0x10], esi
0048d8bf  jbe        0x48d8e9

// block 0048d8c1
0048d8c1  call       0x46e96f

// block 0048d8c6
0048d8c6  push       ebx
0048d8c7  push       ebx
0048d8c8  push       ebx
0048d8c9  push       ebx
0048d8ca  push       ebx
0048d8cb  mov        dword ptr [eax], 0x16
0048d8d1  call       0x470f2d

// block 0048d8d6
0048d8d6  add        esp, 0x14
0048d8d9  cmp        byte ptr [ebp - 4], bl
0048d8dc  je         0x48d8e5

// block 0048d8de
0048d8de  mov        eax, dword ptr [ebp - 8]
0048d8e1  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048d8e5
0048d8e5  mov        eax, esi
0048d8e7  jmp        0x48d949

// block 0048d8e9
0048d8e9  mov        eax, dword ptr [ebp - 0x10]
0048d8ec  cmp        dword ptr [eax + 0x14], ebx
0048d8ef  jne        0x48d90e

// block 0048d8f1
0048d8f1  push       dword ptr [ebp + 0x10]
0048d8f4  push       edi
0048d8f5  push       dword ptr [ebp + 8]
0048d8f8  call       0x48edb0

// block 0048d8fd
0048d8fd  add        esp, 0xc

// block 0048d900
0048d900  cmp        byte ptr [ebp - 4], bl
0048d903  je         0x48d949

// block 0048d905
0048d905  mov        ecx, dword ptr [ebp - 8]
0048d908  and        dword ptr [ecx + 0x70], 0xfffffffd
0048d90c  jmp        0x48d949

// block 0048d90e
0048d90e  mov        eax, dword ptr [ebp + 8]
0048d911  movzx      eax, byte ptr [eax]
0048d914  lea        ecx, [ebp - 0x10]
0048d917  push       ecx
0048d918  push       eax
0048d919  call       0x47aa12

// block 0048d91e
0048d91e  inc        dword ptr [ebp + 8]
0048d921  mov        esi, eax
0048d923  movzx      eax, byte ptr [edi]
0048d926  lea        ecx, [ebp - 0x10]
0048d929  push       ecx
0048d92a  push       eax
0048d92b  call       0x47aa12

// block 0048d930
0048d930  add        esp, 0x10
0048d933  inc        edi
0048d934  dec        dword ptr [ebp + 0x10]
0048d937  je         0x48d941

// block 0048d939
0048d939  cmp        esi, ebx
0048d93b  je         0x48d941

// block 0048d93d
0048d93d  cmp        esi, eax
0048d93f  je         0x48d90e

// block 0048d941
0048d941  sub        esi, eax
0048d943  mov        eax, esi
0048d945  jmp        0x48d900

// block 0048d947
0048d947  xor        eax, eax

// block 0048d949
0048d949  pop        edi
0048d94a  pop        esi
0048d94b  pop        ebx
0048d94c  leave      
0048d94d  ret        
