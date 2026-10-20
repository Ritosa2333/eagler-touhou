
// block 0041c350
0041c350  mov        ecx, dword ptr [eax + 0x30]
0041c353  fldz       
0041c355  xor        edx, edx
0041c357  test       cl, 1
0041c35a  jne        0x41c376

// block 0041c35c
0041c35c  or         ecx, 1
0041c35f  fst        dword ptr [eax + 0x28]
0041c362  mov        dword ptr [eax + 0x24], edx
0041c365  mov        dword ptr [eax + 0x20], 0xfff0bdc1
0041c36c  mov        dword ptr [eax + 0x2c], 0x4b2ed0
0041c373  mov        dword ptr [eax + 0x30], ecx

// block 0041c376
0041c376  fstp       dword ptr [eax + 0x28]
0041c379  mov        dword ptr [eax + 0x24], edx
0041c37c  mov        dword ptr [eax + 0x20], 0xffffffff
0041c383  ret        
