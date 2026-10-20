
// block 0044d140
0044d140  push       esi
0044d141  mov        esi, eax
0044d143  and        dword ptr [esi + 0x20], 0xfffffff8
0044d147  mov        dword ptr [esi + 0x4c], 0
0044d14e  call       0x464c40

// block 0044d153
0044d153  lea        eax, [esi + 8]
0044d156  push       eax
0044d157  push       0
0044d159  push       0
0044d15b  push       0x44d250
0044d160  push       0
0044d162  push       0
0044d164  mov        dword ptr [esi + 0x18], 0x44d250
0044d16b  mov        dword ptr [esi + 0x10], 1
0044d172  mov        dword ptr [esi + 0xc], 0
0044d179  call       0x46e54b

// block 0044d17e
0044d17e  add        esp, 0x18
0044d181  mov        dword ptr [esi + 4], eax
0044d184  pop        esi
0044d185  ret        
