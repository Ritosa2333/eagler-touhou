
// block 0043d6d0
0043d6d0  push       -1
0043d6d2  push       0x496ddb
0043d6d7  mov        eax, dword ptr fs:[0]
0043d6dd  push       eax
0043d6de  push       ebx
0043d6df  push       ebp
0043d6e0  push       esi
0043d6e1  push       edi
0043d6e2  mov        eax, dword ptr [0x4ad138]
0043d6e7  xor        eax, esp
0043d6e9  push       eax
0043d6ea  lea        eax, [esp + 0x14]
0043d6ee  mov        dword ptr fs:[0], eax
0043d6f4  mov        ebx, dword ptr [esp + 0x24]
0043d6f8  mov        dword ptr [esp + 0x1c], 0
0043d700  mov        esi, dword ptr [ebx + 8]
0043d703  mov        edi, dword ptr [0x4ce89c]
0043d709  mov        ebp, dword ptr [0x498088]
0043d70f  test       esi, esi
0043d711  je         0x43d752

// block 0043d713
0043d713  test       dword ptr [0x4cee78], 0x8000
0043d71d  je         0x43d72c

// block 0043d71f
0043d71f  push       0x4cf0f8
0043d724  call       ebp

// block 0043d726
0043d726  inc        byte ptr [0x4cf218]

// block 0043d72c
0043d72c  mov        ecx, esi
0043d72e  mov        edx, edi
0043d730  call       0x462890

// block 0043d735
0043d735  test       dword ptr [0x4cee78], 0x8000
0043d73f  je         0x43d752

// block 0043d741
0043d741  push       0x4cf0f8
0043d746  call       dword ptr [0x49808c]

// block 0043d74c
0043d74c  dec        byte ptr [0x4cf218]

// block 0043d752
0043d752  mov        esi, dword ptr [ebx + 0xc]
0043d755  mov        edi, dword ptr [0x4ce89c]
0043d75b  test       esi, esi
0043d75d  je         0x43d79e

// block 0043d75f
0043d75f  test       dword ptr [0x4cee78], 0x8000
0043d769  je         0x43d778

// block 0043d76b
0043d76b  push       0x4cf0f8
0043d770  call       ebp

// block 0043d772
0043d772  inc        byte ptr [0x4cf218]

// block 0043d778
0043d778  mov        ecx, esi
0043d77a  mov        edx, edi
0043d77c  call       0x462890

// block 0043d781
0043d781  test       dword ptr [0x4cee78], 0x8000
0043d78b  je         0x43d79e

// block 0043d78d
0043d78d  push       0x4cf0f8
0043d792  call       dword ptr [0x49808c]

// block 0043d798
0043d798  dec        byte ptr [0x4cf218]

// block 0043d79e
0043d79e  or         eax, 0xffffffff
0043d7a1  mov        dword ptr [0x4b4520], 0
0043d7ab  call       0x453f10

// block 0043d7b0
0043d7b0  lea        esi, [ebx + 0x10]
0043d7b3  mov        dword ptr [esi], 0x4a3738
0043d7b9  call       0x464c40

// block 0043d7be
0043d7be  mov        ecx, dword ptr [esp + 0x14]
0043d7c2  mov        dword ptr fs:[0], ecx
0043d7c9  pop        ecx
0043d7ca  pop        edi
0043d7cb  pop        esi
0043d7cc  pop        ebp
0043d7cd  pop        ebx
0043d7ce  add        esp, 0xc
0043d7d1  ret        4
