
// block 0045ea40
0045ea40  push       -1
0045ea42  push       0x497217
0045ea47  mov        eax, dword ptr fs:[0]
0045ea4d  push       eax
0045ea4e  push       ebx
0045ea4f  push       esi
0045ea50  push       edi
0045ea51  mov        eax, dword ptr [0x4ad138]
0045ea56  xor        eax, esp
0045ea58  push       eax
0045ea59  lea        eax, [esp + 0x10]
0045ea5d  mov        dword ptr fs:[0], eax
0045ea63  mov        edi, dword ptr [esp + 0x20]
0045ea67  mov        dword ptr [esp + 0x18], 2
0045ea6f  mov        eax, dword ptr [edi + 0x8856b8]
0045ea75  test       eax, eax
0045ea77  je         0x45ea93

// block 0045ea79
0045ea79  lea        esp, [esp]

// block 0045ea80
0045ea80  mov        ebx, dword ptr [eax + 4]
0045ea83  mov        eax, dword ptr [eax]
0045ea85  push       edi
0045ea86  mov        esi, eax
0045ea88  call       0x461450

// block 0045ea8d
0045ea8d  mov        eax, ebx
0045ea8f  test       ebx, ebx
0045ea91  jne        0x45ea80

// block 0045ea93
0045ea93  mov        eax, dword ptr [edi + 0x8856c0]
0045ea99  test       eax, eax
0045ea9b  je         0x45eab3

// block 0045ea9d
0045ea9d  lea        ecx, [ecx]

// block 0045eaa0
0045eaa0  mov        ebx, dword ptr [eax + 4]
0045eaa3  mov        eax, dword ptr [eax]
0045eaa5  push       edi
0045eaa6  mov        esi, eax
0045eaa8  call       0x461450

// block 0045eaad
0045eaad  mov        eax, ebx
0045eaaf  test       ebx, ebx
0045eab1  jne        0x45eaa0

// block 0045eab3
0045eab3  push       0x4026e0
0045eab8  push       0x20
0045eaba  push       0x4b4
0045eabf  lea        eax, [edi + 0x8856c8]
0045eac5  push       eax
0045eac6  mov        byte ptr [esp + 0x28], 1
0045eacb  call       0x46cf1e

// block 0045ead0
0045ead0  mov        eax, dword ptr [edi + 0x4b55f8]
0045ead6  test       eax, eax
0045ead8  je         0x45eae3

// block 0045eada
0045eada  push       eax
0045eadb  call       0x46c941

// block 0045eae0
0045eae0  add        esp, 4

// block 0045eae3
0045eae3  push       0x4026e0
0045eae8  push       0x1000
0045eaed  mov        dword ptr [edi + 0x4b55f8], 0
0045eaf7  push       0x4b4
0045eafc  add        edi, 0xbc
0045eb02  push       edi
0045eb03  mov        dword ptr [esp + 0x28], 0xffffffff
0045eb0b  call       0x46cf1e

// block 0045eb10
0045eb10  mov        ecx, dword ptr [esp + 0x10]
0045eb14  mov        dword ptr fs:[0], ecx
0045eb1b  pop        ecx
0045eb1c  pop        edi
0045eb1d  pop        esi
0045eb1e  pop        ebx
0045eb1f  add        esp, 0xc
0045eb22  ret        4
