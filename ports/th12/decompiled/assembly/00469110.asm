
// block 00469110
00469110  push       ebx
00469111  push       esi
00469112  push       edi
00469113  push       0x1024
00469118  mov        edi, eax
0046911a  call       0x46c9ea

// block 0046911f
0046911f  xor        ebx, ebx
00469121  add        esp, 4
00469124  cmp        eax, ebx
00469126  je         0x469138

// block 00469128
00469128  mov        dword ptr [eax + 0x1008], ebx
0046912e  mov        dword ptr [eax + 0x100c], ebx
00469134  mov        esi, eax
00469136  jmp        0x46913a

// block 00469138
00469138  xor        esi, esi

// block 0046913a
0046913a  push       0xc
0046913c  call       0x46c9ea

// block 00469141
00469141  fldz       
00469143  mov        ecx, dword ptr [esp + 0x14]
00469147  fstp       dword ptr [esi]
00469149  mov        dword ptr [esi + 0x1010], ecx
0046914f  mov        dword ptr [esi + 0x1014], edi
00469155  mov        dword ptr [esi + 4], ebx
00469158  mov        edx, dword ptr [edi + 4]
0046915b  mov        cl, byte ptr [edx + 0x101c]
00469161  mov        byte ptr [esi + 0x101c], cl
00469167  lea        ecx, [edi + 0x1030]
0046916d  mov        dword ptr [eax], esi
0046916f  mov        dword ptr [eax + 4], ebx
00469172  mov        dword ptr [eax + 8], ebx
00469175  mov        edx, dword ptr [ecx + 4]
00469178  add        esp, 4
0046917b  cmp        edx, ebx
0046917d  je         0x469188

// block 0046917f
0046917f  mov        dword ptr [eax + 4], edx
00469182  mov        edx, dword ptr [ecx + 4]
00469185  mov        dword ptr [edx + 8], eax

// block 00469188
00469188  mov        dword ptr [ecx + 4], eax
0046918b  mov        dword ptr [eax + 8], ecx
0046918e  mov        eax, dword ptr [esp + 0x14]
00469192  mov        edi, dword ptr [edi + 4]
00469195  push       eax
00469196  mov        eax, esi
00469198  call       0x466f00

// block 0046919d
0046919d  pop        edi
0046919e  pop        esi
0046919f  pop        ebx
004691a0  ret        8
