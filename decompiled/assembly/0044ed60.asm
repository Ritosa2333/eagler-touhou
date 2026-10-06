
// block 0044ed60
0044ed60  push       ebx
0044ed61  mov        ebx, dword ptr [esp + 8]
0044ed65  push       esi
0044ed66  mov        esi, ecx
0044ed68  push       edi
0044ed69  cmp        dword ptr [esi + 0x14], ebx
0044ed6c  jae        0x44ed73

// block 0044ed6e
0044ed6e  call       0x4916e6

// block 0044ed73
0044ed73  mov        eax, dword ptr [esi + 0x14]
0044ed76  mov        edi, dword ptr [esp + 0x14]
0044ed7a  sub        eax, ebx
0044ed7c  cmp        eax, edi
0044ed7e  jae        0x44ed82

// block 0044ed80
0044ed80  mov        edi, eax

// block 0044ed82
0044ed82  test       edi, edi
0044ed84  jbe        0x44eddb

// block 0044ed86
0044ed86  mov        ecx, dword ptr [esi + 0x18]
0044ed89  push       ebp
0044ed8a  lea        ebp, [esi + 4]
0044ed8d  cmp        ecx, 0x10
0044ed90  jb         0x44ed9b

// block 0044ed92
0044ed92  mov        edx, dword ptr [ebp]
0044ed95  mov        dword ptr [esp + 0x14], edx
0044ed99  jmp        0x44ed9f

// block 0044ed9b
0044ed9b  mov        dword ptr [esp + 0x14], ebp

// block 0044ed9f
0044ed9f  cmp        ecx, 0x10
0044eda2  jb         0x44eda9

// block 0044eda4
0044eda4  mov        edx, dword ptr [ebp]
0044eda7  jmp        0x44edab

// block 0044eda9
0044eda9  mov        edx, ebp

// block 0044edab
0044edab  sub        eax, edi
0044edad  push       eax
0044edae  mov        eax, dword ptr [esp + 0x18]
0044edb2  add        eax, ebx
0044edb4  add        eax, edi
0044edb6  push       eax
0044edb7  sub        ecx, ebx
0044edb9  push       ecx
0044edba  add        edx, ebx
0044edbc  push       edx
0044edbd  call       0x46e28d

// block 0044edc2
0044edc2  mov        eax, dword ptr [esi + 0x14]
0044edc5  sub        eax, edi
0044edc7  add        esp, 0x10
0044edca  cmp        dword ptr [esi + 0x18], 0x10
0044edce  mov        dword ptr [esi + 0x14], eax
0044edd1  jb         0x44edd6

// block 0044edd3
0044edd3  mov        ebp, dword ptr [ebp]

// block 0044edd6
0044edd6  mov        byte ptr [eax + ebp], 0
0044edda  pop        ebp

// block 0044eddb
0044eddb  pop        edi
0044eddc  mov        eax, esi
0044edde  pop        esi
0044eddf  pop        ebx
0044ede0  ret        8
