
// block 00477a40
00477a40  mov        edi, edi
00477a42  push       ebp
00477a43  mov        ebp, esp
00477a45  push       -2
00477a47  push       0x4aae18
00477a4c  push       0x46fd80
00477a51  mov        eax, dword ptr fs:[0]
00477a57  push       eax
00477a58  sub        esp, 8
00477a5b  push       ebx
00477a5c  push       esi
00477a5d  push       edi
00477a5e  mov        eax, dword ptr [0x4ad138]
00477a63  xor        dword ptr [ebp - 8], eax
00477a66  xor        eax, ebp
00477a68  push       eax
00477a69  lea        eax, [ebp - 0x10]
00477a6c  mov        dword ptr fs:[0], eax
00477a72  mov        dword ptr [ebp - 0x18], esp
00477a75  mov        dword ptr [ebp - 4], 0
00477a7c  push       0x400000
00477a81  call       0x4779b0

// block 00477a86
00477a86  add        esp, 4
00477a89  test       eax, eax
00477a8b  je         0x477ae2

// block 00477a8d
00477a8d  mov        eax, dword ptr [ebp + 8]
00477a90  sub        eax, 0x400000
00477a95  push       eax
00477a96  push       0x400000
00477a9b  call       0x4779f0

// block 00477aa0
00477aa0  add        esp, 8
00477aa3  test       eax, eax
00477aa5  je         0x477ae2

// block 00477aa7
00477aa7  mov        eax, dword ptr [eax + 0x24]
00477aaa  shr        eax, 0x1f
00477aad  not        eax
00477aaf  and        eax, 1
00477ab2  mov        dword ptr [ebp - 4], 0xfffffffe
00477ab9  mov        ecx, dword ptr [ebp - 0x10]
00477abc  mov        dword ptr fs:[0], ecx
00477ac3  pop        ecx
00477ac4  pop        edi
00477ac5  pop        esi
00477ac6  pop        ebx
00477ac7  mov        esp, ebp
00477ac9  pop        ebp
00477aca  ret        

// block 00477ae2
00477ae2  mov        dword ptr [ebp - 4], 0xfffffffe
00477ae9  xor        eax, eax
00477aeb  mov        ecx, dword ptr [ebp - 0x10]
00477aee  mov        dword ptr fs:[0], ecx
00477af5  pop        ecx
00477af6  pop        edi
00477af7  pop        esi
00477af8  pop        ebx
00477af9  mov        esp, ebp
00477afb  pop        ebp
00477afc  ret        
