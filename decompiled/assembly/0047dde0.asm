
// block 0047dde0
0047dde0  mov        edi, edi
0047dde2  push       ebp
0047dde3  mov        ebp, esp
0047dde5  mov        eax, ecx
0047dde7  mov        ecx, dword ptr [ebp + 8]
0047ddea  mov        edx, dword ptr [ecx]
0047ddec  mov        dword ptr [eax], edx
0047ddee  mov        ecx, dword ptr [ecx + 4]
0047ddf1  mov        dword ptr [eax + 4], ecx
0047ddf4  pop        ebp
0047ddf5  ret        4
