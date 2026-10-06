
// block 0041cd90
0041cd90  mov        eax, dword ptr [esi + 0x40]
0041cd93  push       edi
0041cd94  push       eax
0041cd95  call       0x461a70

// block 0041cd9a
0041cd9a  xor        edi, edi
0041cd9c  mov        dword ptr [esi + 0x40], edi
0041cd9f  mov        ecx, dword ptr [esi + 0x44]
0041cda2  push       ecx
0041cda3  call       0x461a70

// block 0041cda8
0041cda8  mov        dword ptr [esi + 0x44], edi
0041cdab  mov        edx, dword ptr [esi + 0x48]
0041cdae  push       edx
0041cdaf  call       0x461a70

// block 0041cdb4
0041cdb4  mov        dword ptr [esi + 0x48], edi
0041cdb7  mov        eax, dword ptr [esi + 0x4c]
0041cdba  push       eax
0041cdbb  call       0x461a70

// block 0041cdc0
0041cdc0  mov        dword ptr [esi + 0x4c], edi
0041cdc3  mov        ecx, dword ptr [esi + 0x50]
0041cdc6  push       ecx
0041cdc7  call       0x461a70

// block 0041cdcc
0041cdcc  mov        dword ptr [esi + 0x50], edi
0041cdcf  mov        edx, dword ptr [esi + 0x54]
0041cdd2  push       edx
0041cdd3  call       0x461a70

// block 0041cdd8
0041cdd8  mov        dword ptr [esi + 0x54], edi
0041cddb  mov        eax, dword ptr [esi + 0x58]
0041cdde  push       eax
0041cddf  call       0x461a70

// block 0041cde4
0041cde4  mov        dword ptr [esi + 0x58], edi
0041cde7  mov        ecx, dword ptr [esi + 0x5c]
0041cdea  push       ecx
0041cdeb  call       0x461a70

// block 0041cdf0
0041cdf0  mov        dword ptr [esi + 0x5c], edi
0041cdf3  pop        edi
0041cdf4  ret        
