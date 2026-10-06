
// block 0045cdf0
0045cdf0  mov        edx, dword ptr [eax + 0x47c]
0045cdf6  shr        edx, 0x17
0045cdf9  push       ebx
0045cdfa  and        edx, 0x1f
0045cdfd  push       esi
0045cdfe  push       edi
0045cdff  cmp        edx, 3
0045ce02  ja         0x45ce45

// block 0045ce04
0045ce04  jmp        dword ptr [edx*4 + 0x45ce4c]

// block 0045ce0b
0045ce0b  lea        edx, [ecx + 0xc]
0045ce0e  push       edx
0045ce0f  lea        edi, [ecx + 0x24]
0045ce12  lea        ebx, [ecx + 0x18]
0045ce15  push       ecx
0045ce16  mov        esi, eax
0045ce18  call       0x45a570

// block 0045ce1d
0045ce1d  pop        edi
0045ce1e  pop        esi
0045ce1f  pop        ebx
0045ce20  ret        

// block 0045ce21
0045ce21  lea        esi, [ecx + 0x24]
0045ce24  lea        edi, [ecx + 0x18]
0045ce27  lea        ebx, [ecx + 0xc]
0045ce2a  push       ecx
0045ce2b  call       0x45af10

// block 0045ce30
0045ce30  pop        edi
0045ce31  pop        esi
0045ce32  pop        ebx
0045ce33  ret        

// block 0045ce34
0045ce34  lea        edx, [ecx + 0x24]
0045ce37  lea        esi, [ecx + 0x18]
0045ce3a  lea        edi, [ecx + 0xc]
0045ce3d  push       eax
0045ce3e  mov        ebx, ecx
0045ce40  call       0x45aa30

// block 0045ce45
0045ce45  pop        edi
0045ce46  pop        esi
0045ce47  pop        ebx
0045ce48  ret        
