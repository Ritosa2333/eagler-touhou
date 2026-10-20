
// block 0041ad90
0041ad90  mov        eax, dword ptr [0x4b43dc]
0041ad95  mov        eax, dword ptr [eax + 0x68]
0041ad98  test       eax, eax
0041ad9a  je         0x41adc3

// block 0041ad9c
0041ad9c  mov        edx, 0x400

// block 0041ada1
0041ada1  mov        ecx, dword ptr [eax + 4]
0041ada4  mov        eax, dword ptr [eax]
0041ada6  add        eax, 0x1040
0041adab  test       dword ptr [eax + 0x16b8], edx
0041adb1  je         0x41adbd

// block 0041adb3
0041adb3  mov        dword ptr [eax + 0x240], 1

// block 0041adbd
0041adbd  mov        eax, ecx
0041adbf  test       ecx, ecx
0041adc1  jne        0x41ada1

// block 0041adc3
0041adc3  xor        eax, eax
0041adc5  ret        
