
// block 0041c6a0
0041c6a0  sub        esp, 0x10
0041c6a3  test       dword ptr [0x4cee78], 0x8000
0041c6ad  push       ebx
0041c6ae  push       ebp
0041c6af  mov        ebp, dword ptr [esp + 0x1c]
0041c6b3  push       esi
0041c6b4  mov        ebx, eax
0041c6b6  je         0x41c6c9

// block 0041c6b8
0041c6b8  push       0x4cf1d0
0041c6bd  call       dword ptr [0x498088]

// block 0041c6c3
0041c6c3  inc        byte ptr [0x4cf221]

// block 0041c6c9
0041c6c9  inc        dword ptr [edi + 0x130]
0041c6cf  call       0x4621c0

// block 0041c6d4
0041c6d4  mov        esi, eax
0041c6d6  test       ebx, ebx
0041c6d8  jl         0x41c6dd

// block 0041c6da
0041c6da  mov        dword ptr [esi + 0x20], ebx

// block 0041c6dd
0041c6dd  fldz       
0041c6df  or         dword ptr [esi + 0x480], 1
0041c6e6  fst        dword ptr [esp + 0x10]
0041c6ea  mov        eax, dword ptr [esp + 0x10]
0041c6ee  fst        dword ptr [esp + 0x14]
0041c6f2  mov        ecx, dword ptr [esp + 0x14]
0041c6f6  fstp       dword ptr [esp + 0x18]
0041c6fa  mov        edx, dword ptr [esp + 0x18]
0041c6fe  mov        dword ptr [esi + 0x430], eax
0041c704  mov        eax, dword ptr [esp + 0x24]
0041c708  mov        dword ptr [esi + 0x434], ecx
0041c70e  push       eax
0041c70f  push       esi
0041c710  mov        ecx, edi
0041c712  mov        dword ptr [esi + 0x438], edx
0041c718  call       0x454d10

// block 0041c71d
0041c71d  mov        ebx, esi
0041c71f  lea        eax, [esp + 0x20]
0041c723  call       0x4612d0

// block 0041c728
0041c728  test       dword ptr [0x4cee78], 0x8000
0041c732  mov        ecx, dword ptr [eax]
0041c734  mov        dword ptr [ebp], ecx
0041c737  je         0x41c74a

// block 0041c739
0041c739  push       0x4cf1d0
0041c73e  call       dword ptr [0x49808c]

// block 0041c744
0041c744  dec        byte ptr [0x4cf221]

// block 0041c74a
0041c74a  pop        esi
0041c74b  mov        eax, ebp
0041c74d  pop        ebp
0041c74e  pop        ebx
0041c74f  add        esp, 0x10
0041c752  ret        8
