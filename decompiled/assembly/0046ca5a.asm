
// block 0046ca5a
0046ca5a  mov        edi, edi
0046ca5c  push       ebp
0046ca5d  mov        ebp, esp
0046ca5f  sub        esp, 0x20
0046ca62  push       ebx
0046ca63  xor        ebx, ebx
0046ca65  cmp        dword ptr [ebp + 0xc], ebx
0046ca68  jne        0x46ca87

// block 0046ca6a
0046ca6a  call       0x46e96f

// block 0046ca6f
0046ca6f  push       ebx
0046ca70  push       ebx
0046ca71  push       ebx
0046ca72  push       ebx
0046ca73  push       ebx
0046ca74  mov        dword ptr [eax], 0x16
0046ca7a  call       0x470f2d

// block 0046ca7f
0046ca7f  add        esp, 0x14
0046ca82  or         eax, 0xffffffff
0046ca85  jmp        0x46cad5

// block 0046ca87
0046ca87  mov        eax, dword ptr [ebp + 8]
0046ca8a  cmp        eax, ebx
0046ca8c  je         0x46ca6a

// block 0046ca8e
0046ca8e  push       esi
0046ca8f  push       dword ptr [ebp + 0x14]
0046ca92  mov        dword ptr [ebp - 0x18], eax
0046ca95  push       dword ptr [ebp + 0x10]
0046ca98  mov        dword ptr [ebp - 0x20], eax
0046ca9b  push       dword ptr [ebp + 0xc]
0046ca9e  lea        eax, [ebp - 0x20]
0046caa1  push       eax
0046caa2  mov        dword ptr [ebp - 0x1c], 0x7fffffff
0046caa9  mov        dword ptr [ebp - 0x14], 0x42
0046cab0  call       0x47021f

// block 0046cab5
0046cab5  add        esp, 0x10
0046cab8  dec        dword ptr [ebp - 0x1c]
0046cabb  mov        esi, eax
0046cabd  js         0x46cac6

// block 0046cabf
0046cabf  mov        eax, dword ptr [ebp - 0x20]
0046cac2  mov        byte ptr [eax], bl
0046cac4  jmp        0x46cad2

// block 0046cac6
0046cac6  lea        eax, [ebp - 0x20]
0046cac9  push       eax
0046caca  push       ebx
0046cacb  call       0x46ffdb

// block 0046cad0
0046cad0  pop        ecx
0046cad1  pop        ecx

// block 0046cad2
0046cad2  mov        eax, esi
0046cad4  pop        esi

// block 0046cad5
0046cad5  pop        ebx
0046cad6  leave      
0046cad7  ret        
