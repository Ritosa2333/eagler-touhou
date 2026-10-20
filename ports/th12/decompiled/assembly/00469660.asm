
// block 00469660
00469660  mov        ecx, dword ptr [eax + 0x1000]
00469666  add        ecx, -4
00469669  jns        0x46966f

// block 0046966b
0046966b  or         eax, 0xffffffff
0046966e  ret        

// block 0046966f
0046966f  mov        dword ptr [eax + 0x1000], ecx
00469675  mov        ecx, dword ptr [ecx + eax]
00469678  mov        dword ptr [esi], ecx
0046967a  test       dl, dl
0046967c  je         0x4696b0

// block 0046967e
0046967e  add        dword ptr [eax + 0x1000], -4
00469685  mov        ecx, dword ptr [eax + 0x1000]
0046968b  mov        al, byte ptr [ecx + eax]
0046968e  cmp        al, 0x66
00469690  je         0x4696a2

// block 00469692
00469692  cmp        al, 0x69
00469694  jne        0x4696b0

// block 00469696
00469696  cmp        dl, 0x66
00469699  jne        0x4696b0

// block 0046969b
0046969b  fild       dword ptr [esi]
0046969d  xor        eax, eax
0046969f  fstp       dword ptr [esi]
004696a1  ret        

// block 004696a2
004696a2  cmp        dl, 0x69
004696a5  jne        0x4696b0

// block 004696a7
004696a7  fld        dword ptr [esi]
004696a9  call       0x4931e0

// block 004696ae
004696ae  mov        dword ptr [esi], eax

// block 004696b0
004696b0  xor        eax, eax
004696b2  ret        
