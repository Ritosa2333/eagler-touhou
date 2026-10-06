
// block 0040fd40
0040fd40  push       esi
0040fd41  push       edi
0040fd42  push       0x34
0040fd44  mov        esi, edx
0040fd46  mov        edi, ecx
0040fd48  call       0x46c9ea

// block 0040fd4d
0040fd4d  mov        dword ptr [edi + 0x478], eax
0040fd53  mov        ecx, dword ptr [esi]
0040fd55  mov        dword ptr [eax], ecx
0040fd57  mov        edx, dword ptr [esi + 4]
0040fd5a  mov        dword ptr [eax + 4], edx
0040fd5d  mov        ecx, dword ptr [esi + 8]
0040fd60  mov        dword ptr [eax + 8], ecx
0040fd63  mov        edx, dword ptr [esi + 0xc]
0040fd66  mov        dword ptr [eax + 0xc], edx
0040fd69  mov        ecx, dword ptr [esi + 0x10]
0040fd6c  mov        dword ptr [eax + 0x10], ecx
0040fd6f  mov        edx, dword ptr [esi + 0x14]
0040fd72  mov        dword ptr [eax + 0x14], edx
0040fd75  fld        dword ptr [esi + 0x18]
0040fd78  fstp       dword ptr [eax + 0x18]
0040fd7b  add        esp, 4
0040fd7e  fld        dword ptr [esi + 0x1c]
0040fd81  fstp       dword ptr [eax + 0x1c]
0040fd84  mov        edx, dword ptr [eax]
0040fd86  fld        dword ptr [esi + 0x20]
0040fd89  fstp       dword ptr [eax + 0x20]
0040fd8c  fld        dword ptr [esi + 0x24]
0040fd8f  fstp       dword ptr [eax + 0x24]
0040fd92  fld        dword ptr [esi + 0x28]
0040fd95  fstp       dword ptr [eax + 0x28]
0040fd98  fld        dword ptr [esi + 0x2c]
0040fd9b  fstp       dword ptr [eax + 0x2c]
0040fd9e  mov        ecx, dword ptr [esi + 0x30]
0040fda1  mov        dword ptr [eax + 0x30], ecx
0040fda4  mov        dword ptr [edi + 0x430], edx
0040fdaa  mov        ecx, dword ptr [eax + 4]
0040fdad  mov        dword ptr [edi + 0x434], ecx
0040fdb3  mov        edx, dword ptr [eax + 8]
0040fdb6  mov        dword ptr [edi + 0x438], edx
0040fdbc  pop        edi
0040fdbd  xor        eax, eax
0040fdbf  pop        esi
0040fdc0  ret        
