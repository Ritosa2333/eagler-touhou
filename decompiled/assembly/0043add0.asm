
// block 0043add0
0043add0  mov        eax, dword ptr [0x4b4514]
0043add5  mov        edx, dword ptr [ecx]
0043add7  mov        dword ptr [eax + 0x988], edx
0043addd  mov        ecx, dword ptr [ecx + 4]
0043ade0  mov        dword ptr [eax + 0x98c], ecx
0043ade6  fild       dword ptr [eax + 0x988]
0043adec  fld        qword ptr [0x4a3d40]
0043adf2  mov        ecx, 1
0043adf7  fmul       st(1), st(0)
0043adf9  fxch       st(1)
0043adfb  fstp       dword ptr [eax + 0x97c]
0043ae01  fimul      dword ptr [eax + 0x98c]
0043ae07  fstp       dword ptr [eax + 0x980]
0043ae0d  mov        dword ptr [eax + 0x8334], ecx
0043ae13  mov        dword ptr [eax + 0x8418], ecx
0043ae19  mov        dword ptr [eax + 0x84fc], ecx
0043ae1f  mov        dword ptr [eax + 0x85e0], ecx
0043ae25  mov        dword ptr [eax + 0x86c4], ecx
0043ae2b  mov        dword ptr [eax + 0x87a8], ecx
0043ae31  mov        dword ptr [eax + 0x888c], ecx
0043ae37  mov        dword ptr [eax + 0x8970], ecx
0043ae3d  ret        
