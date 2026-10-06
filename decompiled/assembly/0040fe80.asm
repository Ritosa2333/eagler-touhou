
// block 0040fe80
0040fe80  push       0x34
0040fe82  call       0x46c9ea

// block 0040fe87
0040fe87  mov        dword ptr [edi + 0x478], eax
0040fe8d  mov        ecx, dword ptr [esi]
0040fe8f  mov        dword ptr [eax], ecx
0040fe91  mov        edx, dword ptr [esi + 4]
0040fe94  mov        dword ptr [eax + 4], edx
0040fe97  mov        ecx, dword ptr [esi + 8]
0040fe9a  mov        dword ptr [eax + 8], ecx
0040fe9d  mov        edx, dword ptr [esi + 0xc]
0040fea0  mov        dword ptr [eax + 0xc], edx
0040fea3  mov        ecx, dword ptr [esi + 0x10]
0040fea6  mov        dword ptr [eax + 0x10], ecx
0040fea9  mov        edx, dword ptr [esi + 0x14]
0040feac  mov        dword ptr [eax + 0x14], edx
0040feaf  fld        dword ptr [esi + 0x18]
0040feb2  fstp       dword ptr [eax + 0x18]
0040feb5  add        esp, 4
0040feb8  fld        dword ptr [esi + 0x1c]
0040febb  fstp       dword ptr [eax + 0x1c]
0040febe  mov        edx, dword ptr [eax]
0040fec0  fld        dword ptr [esi + 0x20]
0040fec3  fstp       dword ptr [eax + 0x20]
0040fec6  fld        dword ptr [esi + 0x24]
0040fec9  fstp       dword ptr [eax + 0x24]
0040fecc  fld        dword ptr [esi + 0x28]
0040fecf  fstp       dword ptr [eax + 0x28]
0040fed2  fld        dword ptr [esi + 0x2c]
0040fed5  fstp       dword ptr [eax + 0x2c]
0040fed8  mov        ecx, dword ptr [esi + 0x30]
0040fedb  mov        dword ptr [eax + 0x30], ecx
0040fede  mov        dword ptr [edi + 0x430], edx
0040fee4  mov        ecx, dword ptr [eax + 4]
0040fee7  mov        dword ptr [edi + 0x434], ecx
0040feed  mov        edx, dword ptr [eax + 8]
0040fef0  mov        dword ptr [edi + 0x438], edx
0040fef6  xor        eax, eax
0040fef8  ret        
