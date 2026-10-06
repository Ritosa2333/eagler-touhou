
// block 0046abe0
0046abe0  sub        esp, 0x18
0046abe3  fld        dword ptr [0x4a3f4c]
0046abe9  mov        ecx, dword ptr [0x4b43cc]
0046abef  mov        edx, dword ptr [ecx + 0x7c]
0046abf2  fstp       dword ptr [esp]
0046abf5  fld        dword ptr [0x4a434c]
0046abfb  push       esi
0046abfc  fstp       dword ptr [esp + 8]
0046ac00  not        edx
0046ac02  fldz       
0046ac04  push       edi
0046ac05  fst        dword ptr [esp + 0x10]
0046ac09  and        edx, 1
0046ac0c  fld        dword ptr [eax + 0x508]
0046ac12  push       edx
0046ac13  fstp       dword ptr [esp + 0x18]
0046ac17  lea        eax, [esp + 0xc]
0046ac1b  fld        dword ptr [0x4a40d4]
0046ac21  push       eax
0046ac22  fstp       dword ptr [esp + 0x20]
0046ac26  lea        eax, [esp + 0x1c]
0046ac2a  fstp       dword ptr [esp + 0x24]
0046ac2e  call       0x40cd00

// block 0046ac33
0046ac33  mov        ecx, dword ptr [0x4b43cc]
0046ac39  mov        edx, dword ptr [ecx + 0x7c]
0046ac3c  not        edx
0046ac3e  and        edx, 1
0046ac41  push       edx
0046ac42  lea        esi, [esp + 0xc]
0046ac46  lea        edi, [esp + 0x18]
0046ac4a  call       0x4285f0

// block 0046ac4f
0046ac4f  pop        edi
0046ac50  xor        eax, eax
0046ac52  pop        esi
0046ac53  add        esp, 0x18
0046ac56  ret        
