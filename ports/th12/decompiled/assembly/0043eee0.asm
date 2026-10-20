
// block 0043eee0
0043eee0  mov        ecx, dword ptr [eax + 0x1c]
0043eee3  fldz       
0043eee5  mov        dword ptr [eax + 0x1c], edx
0043eee8  xor        edx, edx
0043eeea  mov        dword ptr [eax + 0x20], ecx
0043eeed  mov        dword ptr [eax + 0x24], edx
0043eef0  mov        ecx, dword ptr [eax + 0x2c4]
0043eef6  test       cl, 1
0043eef9  jne        0x43ef24

// block 0043eefb
0043eefb  or         ecx, 1
0043eefe  fst        dword ptr [eax + 0x2bc]
0043ef04  mov        dword ptr [eax + 0x2b8], edx
0043ef0a  mov        dword ptr [eax + 0x2b4], 0xfff0bdc1
0043ef14  mov        dword ptr [eax + 0x2c0], 0x4b2ed0
0043ef1e  mov        dword ptr [eax + 0x2c4], ecx

// block 0043ef24
0043ef24  fstp       dword ptr [eax + 0x2bc]
0043ef2a  mov        dword ptr [eax + 0x2b8], edx
0043ef30  mov        dword ptr [eax + 0x2b4], 0xffffffff
0043ef3a  ret        
