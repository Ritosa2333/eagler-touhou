
// block 00452a20
00452a20  fldz       
00452a22  mov        dword ptr [eax + 0x2c], 1
00452a29  mov        ecx, dword ptr [eax + 0x40]
00452a2c  xor        edx, edx
00452a2e  test       cl, 1
00452a31  jne        0x452a4d

// block 00452a33
00452a33  or         ecx, 1
00452a36  fst        dword ptr [eax + 0x38]
00452a39  mov        dword ptr [eax + 0x34], edx
00452a3c  mov        dword ptr [eax + 0x30], 0xfff0bdc1
00452a43  mov        dword ptr [eax + 0x3c], 0x4b2ed0
00452a4a  mov        dword ptr [eax + 0x40], ecx

// block 00452a4d
00452a4d  fstp       dword ptr [eax + 0x38]
00452a50  mov        dword ptr [eax + 0x34], edx
00452a53  mov        dword ptr [eax + 0x30], 0xffffffff
00452a5a  ret        
