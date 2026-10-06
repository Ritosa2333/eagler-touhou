
// block 00425670
00425670  mov        eax, dword ptr [0x4b0cb0]
00425675  lea        eax, [eax + eax*2]
00425678  mov        eax, dword ptr [ecx + eax*4 + 0x7c]
0042567c  test       eax, eax
0042567e  je         0x425696

// block 00425680
00425680  mov        edx, dword ptr [eax + 4]
00425683  mov        eax, dword ptr [eax]
00425685  mov        ecx, dword ptr [eax + 0x70]
00425688  test       ecx, ecx
0042568a  jle        0x425690

// block 0042568c
0042568c  dec        ecx
0042568d  mov        dword ptr [eax + 0x70], ecx

// block 00425690
00425690  mov        eax, edx
00425692  test       edx, edx
00425694  jne        0x425680

// block 00425696
00425696  ret        
