
// block 0043d600
0043d600  push       ebx
0043d601  push       ebp
0043d602  mov        ebp, dword ptr [esp + 0xc]
0043d606  push       esi
0043d607  push       edi
0043d608  xor        edi, edi
0043d60a  push       0x24
0043d60c  mov        dword ptr [0x4b452c], 0x4aebf0
0043d616  mov        dword ptr [0x4b0cb0], edi
0043d61c  mov        dword ptr [0x4b0cb4], edi
0043d622  call       0x46c9ea

// block 0043d627
0043d627  add        esp, 4
0043d62a  cmp        eax, edi
0043d62c  je         0x43d64a

// block 0043d62e
0043d62e  and        dword ptr [eax + 4], 0xfffffffe
0043d632  mov        dword ptr [eax + 8], edi
0043d635  mov        dword ptr [eax + 0xc], edi
0043d638  mov        dword ptr [eax + 0x10], edi
0043d63b  mov        dword ptr [eax], edi
0043d63d  mov        dword ptr [eax + 0x14], eax
0043d640  mov        dword ptr [eax + 0x18], edi
0043d643  mov        dword ptr [eax + 0x1c], edi
0043d646  mov        esi, eax
0043d648  jmp        0x43d64c

// block 0043d64a
0043d64a  xor        esi, esi

// block 0043d64c
0043d64c  or         dword ptr [esi + 4], 3
0043d650  mov        ebx, 7
0043d655  mov        dword ptr [esi + 8], 0x43dac0
0043d65c  mov        dword ptr [esi + 0xc], edi
0043d65f  mov        dword ptr [esi + 0x10], edi
0043d662  mov        dword ptr [esi + 0x20], ebp
0043d665  call       0x462380

// block 0043d66a
0043d66a  push       0x24
0043d66c  mov        dword ptr [ebp + 8], esi
0043d66f  call       0x46c9ea

// block 0043d674
0043d674  add        esp, 4
0043d677  cmp        eax, edi
0043d679  je         0x43d697

// block 0043d67b
0043d67b  and        dword ptr [eax + 4], 0xfffffffe
0043d67f  mov        dword ptr [eax + 8], edi
0043d682  mov        dword ptr [eax + 0xc], edi
0043d685  mov        dword ptr [eax + 0x10], edi
0043d688  mov        dword ptr [eax], edi
0043d68a  mov        dword ptr [eax + 0x14], eax
0043d68d  mov        dword ptr [eax + 0x18], edi
0043d690  mov        dword ptr [eax + 0x1c], edi
0043d693  mov        esi, eax
0043d695  jmp        0x43d699

// block 0043d697
0043d697  xor        esi, esi

// block 0043d699
0043d699  or         dword ptr [esi + 4], 3
0043d69d  mov        ebx, 0x44
0043d6a2  mov        dword ptr [esi + 8], 0x43dad0
0043d6a9  mov        dword ptr [esi + 0xc], edi
0043d6ac  mov        dword ptr [esi + 0x10], edi
0043d6af  mov        dword ptr [esi + 0x20], ebp
0043d6b2  call       0x462420

// block 0043d6b7
0043d6b7  pop        edi
0043d6b8  mov        dword ptr [ebp + 0xc], esi
0043d6bb  pop        esi
0043d6bc  pop        ebp
0043d6bd  xor        eax, eax
0043d6bf  pop        ebx
0043d6c0  ret        4
