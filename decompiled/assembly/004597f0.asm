
// block 004597f0
004597f0  mov        ecx, dword ptr [eax + 0x40]
004597f3  fldz       
004597f5  xor        edx, edx
004597f7  test       cl, 1
004597fa  jne        0x459816

// block 004597fc
004597fc  or         ecx, 1
004597ff  fst        dword ptr [eax + 0x38]
00459802  mov        dword ptr [eax + 0x34], edx
00459805  mov        dword ptr [eax + 0x30], 0xfff0bdc1
0045980c  mov        dword ptr [eax + 0x3c], 0x4b2ed0
00459813  mov        dword ptr [eax + 0x40], ecx

// block 00459816
00459816  fstp       dword ptr [eax + 0x38]
00459819  mov        dword ptr [eax + 0x34], edx
0045981c  mov        dword ptr [eax + 0x30], 0xffffffff
00459823  ret        
