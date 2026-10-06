
// block 0043b450
0043b450  push       -1
0043b452  push       0x497078
0043b457  mov        eax, dword ptr fs:[0]
0043b45d  push       eax
0043b45e  push       ebx
0043b45f  push       ebp
0043b460  push       esi
0043b461  push       edi
0043b462  mov        eax, dword ptr [0x4ad138]
0043b467  xor        eax, esp
0043b469  push       eax
0043b46a  lea        eax, [esp + 0x14]
0043b46e  mov        dword ptr fs:[0], eax
0043b474  mov        ebx, dword ptr [esp + 0x24]
0043b478  xor        ebp, ebp
0043b47a  mov        dword ptr [esp + 0x1c], ebp
0043b47e  mov        eax, dword ptr [ebx + 0x18]
0043b481  push       eax
0043b482  call       0x46ca4f

// block 0043b487
0043b487  add        esp, 4
0043b48a  xor        esi, esi
0043b48c  lea        esp, [esp]

// block 0043b490
0043b490  mov        eax, esi
0043b492  mov        ecx, ebx
0043b494  call       0x43ccd0

// block 0043b499
0043b499  inc        esi
0043b49a  cmp        esi, 8
0043b49d  jl         0x43b490

// block 0043b49f
0043b49f  mov        eax, dword ptr [ebx + 0x1c]
0043b4a2  push       eax
0043b4a3  call       0x46ca4f

// block 0043b4a8
0043b4a8  add        esp, 4
0043b4ab  mov        dword ptr [ebx + 0x1c], ebp
0043b4ae  lea        esi, [ebx + 0x20]
0043b4b1  mov        edi, 8

// block 0043b4b6
0043b4b6  mov        eax, dword ptr [esi]
0043b4b8  push       eax
0043b4b9  call       0x46ca4f

// block 0043b4be
0043b4be  mov        dword ptr [esi], ebp
0043b4c0  add        esp, 4
0043b4c3  add        esi, 4
0043b4c6  sub        edi, 1
0043b4c9  jne        0x43b4b6

// block 0043b4cb
0043b4cb  mov        esi, dword ptr [ebx + 8]
0043b4ce  cmp        esi, ebp
0043b4d0  mov        ebp, dword ptr [0x498088]
0043b4d6  je         0x43b51d

// block 0043b4d8
0043b4d8  test       dword ptr [0x4cee78], 0x8000
0043b4e2  mov        edi, dword ptr [0x4ce89c]
0043b4e8  je         0x43b4f7

// block 0043b4ea
0043b4ea  push       0x4cf0f8
0043b4ef  call       ebp

// block 0043b4f1
0043b4f1  inc        byte ptr [0x4cf218]

// block 0043b4f7
0043b4f7  mov        ecx, esi
0043b4f9  mov        edx, edi
0043b4fb  call       0x462890

// block 0043b500
0043b500  test       dword ptr [0x4cee78], 0x8000
0043b50a  je         0x43b51d

// block 0043b50c
0043b50c  push       0x4cf0f8
0043b511  call       dword ptr [0x49808c]

// block 0043b517
0043b517  dec        byte ptr [0x4cf218]

// block 0043b51d
0043b51d  mov        esi, dword ptr [ebx + 0x1d4]
0043b523  test       esi, esi
0043b525  je         0x43b56c

// block 0043b527
0043b527  test       dword ptr [0x4cee78], 0x8000
0043b531  mov        edi, dword ptr [0x4ce89c]
0043b537  je         0x43b546

// block 0043b539
0043b539  push       0x4cf0f8
0043b53e  call       ebp

// block 0043b540
0043b540  inc        byte ptr [0x4cf218]

// block 0043b546
0043b546  mov        ecx, esi
0043b548  mov        edx, edi
0043b54a  call       0x462890

// block 0043b54f
0043b54f  test       dword ptr [0x4cee78], 0x8000
0043b559  je         0x43b56c

// block 0043b55b
0043b55b  push       0x4cf0f8
0043b560  call       dword ptr [0x49808c]

// block 0043b566
0043b566  dec        byte ptr [0x4cf218]

// block 0043b56c
0043b56c  mov        esi, dword ptr [ebx + 0xc]
0043b56f  test       esi, esi
0043b571  je         0x43b5b8

// block 0043b573
0043b573  test       dword ptr [0x4cee78], 0x8000
0043b57d  mov        edi, dword ptr [0x4ce89c]
0043b583  je         0x43b592

// block 0043b585
0043b585  push       0x4cf0f8
0043b58a  call       ebp

// block 0043b58c
0043b58c  inc        byte ptr [0x4cf218]

// block 0043b592
0043b592  mov        ecx, esi
0043b594  mov        edx, edi
0043b596  call       0x462890

// block 0043b59b
0043b59b  test       dword ptr [0x4cee78], 0x8000
0043b5a5  je         0x43b5b8

// block 0043b5a7
0043b5a7  push       0x4cf0f8
0043b5ac  call       dword ptr [0x49808c]

// block 0043b5b2
0043b5b2  dec        byte ptr [0x4cf218]

// block 0043b5b8
0043b5b8  cmp        ebx, dword ptr [0x4b4518]
0043b5be  jne        0x43b5ca

// block 0043b5c0
0043b5c0  mov        dword ptr [0x4b4518], 0

// block 0043b5ca
0043b5ca  push       0x43cd80
0043b5cf  push       8
0043b5d1  push       0x24
0043b5d3  add        ebx, 0xa8
0043b5d9  push       ebx
0043b5da  mov        dword ptr [esp + 0x2c], 0xffffffff
0043b5e2  call       0x46cf1e

// block 0043b5e7
0043b5e7  mov        ecx, dword ptr [esp + 0x14]
0043b5eb  mov        dword ptr fs:[0], ecx
0043b5f2  pop        ecx
0043b5f3  pop        edi
0043b5f4  pop        esi
0043b5f5  pop        ebp
0043b5f6  pop        ebx
0043b5f7  add        esp, 0xc
0043b5fa  ret        4
