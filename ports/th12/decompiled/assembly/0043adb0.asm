
// block 0043adb0
0043adb0  push       ecx
0043adb1  neg        eax
0043adb3  mov        dword ptr [esp], eax
0043adb6  fild       dword ptr [esp]
0043adb9  push       ecx
0043adba  fstp       dword ptr [esp]
0043adbd  call       0x464a20

// block 0043adc2
0043adc2  pop        ecx
0043adc3  ret        
