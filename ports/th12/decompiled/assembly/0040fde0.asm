
// block 0040fde0
0040fde0  mov        edx, dword ptr [ecx]
0040fde2  mov        dword ptr [eax], edx
0040fde4  mov        edx, dword ptr [ecx + 4]
0040fde7  mov        dword ptr [eax + 4], edx
0040fdea  mov        edx, dword ptr [ecx + 8]
0040fded  mov        dword ptr [eax + 8], edx
0040fdf0  mov        edx, dword ptr [ecx + 0xc]
0040fdf3  mov        dword ptr [eax + 0xc], edx
0040fdf6  mov        edx, dword ptr [ecx + 0x10]
0040fdf9  mov        dword ptr [eax + 0x10], edx
0040fdfc  mov        edx, dword ptr [ecx + 0x14]
0040fdff  mov        dword ptr [eax + 0x14], edx
0040fe02  fld        dword ptr [ecx + 0x18]
0040fe05  fstp       dword ptr [eax + 0x18]
0040fe08  fld        dword ptr [ecx + 0x1c]
0040fe0b  fstp       dword ptr [eax + 0x1c]
0040fe0e  fld        dword ptr [ecx + 0x20]
0040fe11  fstp       dword ptr [eax + 0x20]
0040fe14  fld        dword ptr [ecx + 0x24]
0040fe17  fstp       dword ptr [eax + 0x24]
0040fe1a  fld        dword ptr [ecx + 0x28]
0040fe1d  fstp       dword ptr [eax + 0x28]
0040fe20  fld        dword ptr [ecx + 0x2c]
0040fe23  fstp       dword ptr [eax + 0x2c]
0040fe26  mov        ecx, dword ptr [ecx + 0x30]
0040fe29  mov        dword ptr [eax + 0x30], ecx
0040fe2c  ret        
