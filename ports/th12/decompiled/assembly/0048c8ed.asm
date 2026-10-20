
// block 0048c8ed
0048c8ed  push       ebx
0048c8ee  push       esi
0048c8ef  push       edi
0048c8f0  mov        eax, dword ptr [esp + 0x10]
0048c8f4  push       ebp
0048c8f5  push       eax
0048c8f6  push       -2
0048c8f8  push       0x48c8a8
0048c8fd  push       dword ptr fs:[0]
0048c904  mov        eax, dword ptr [0x4ad138]
0048c909  xor        eax, esp
0048c90b  push       eax
0048c90c  lea        eax, [esp + 4]
0048c910  mov        dword ptr fs:[0], eax

// block 0048c916
0048c916  mov        eax, dword ptr [esp + 0x28]
0048c91a  mov        ebx, dword ptr [eax + 8]
0048c91d  mov        esi, dword ptr [eax + 0xc]
0048c920  cmp        esi, -1
0048c923  je         0x48c95f

// block 0048c925
0048c925  cmp        dword ptr [esp + 0x2c], -1
0048c92a  je         0x48c932

// block 0048c92c
0048c92c  cmp        esi, dword ptr [esp + 0x2c]
0048c930  jbe        0x48c95f

// block 0048c932
0048c932  lea        esi, [esi + esi*2]
0048c935  mov        ecx, dword ptr [ebx + esi*4]
0048c938  mov        dword ptr [esp + 0xc], ecx
0048c93c  mov        dword ptr [eax + 0xc], ecx
0048c93f  cmp        dword ptr [ebx + esi*4 + 4], 0
0048c944  jne        0x48c95d

// block 0048c946
0048c946  push       0x101
0048c94b  mov        eax, dword ptr [ebx + esi*4 + 8]
0048c94f  call       0x48c99d

// block 0048c954
0048c954  mov        eax, dword ptr [ebx + esi*4 + 8]
0048c958  call       0x48c9bc

// block 0048c95d
0048c95d  jmp        0x48c916

// block 0048c95f
0048c95f  mov        ecx, dword ptr [esp + 4]
0048c963  mov        dword ptr fs:[0], ecx
0048c96a  add        esp, 0x18
0048c96d  pop        edi
0048c96e  pop        esi
0048c96f  pop        ebx
0048c970  ret        
