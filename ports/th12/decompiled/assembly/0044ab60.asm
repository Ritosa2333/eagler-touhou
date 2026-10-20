
// block 0044ab60
0044ab60  push       ebp
0044ab61  mov        ebp, esp
0044ab63  and        esp, 0xfffffff8
0044ab66  mov        ecx, dword ptr [edx + 0x38]
0044ab69  sub        esp, 0xc
0044ab6c  push       edi
0044ab6d  test       ecx, ecx
0044ab6f  je         0x44ab91

// block 0044ab71
0044ab71  mov        eax, dword ptr [0x4b43dc]
0044ab76  mov        eax, dword ptr [eax + 0x68]
0044ab79  test       eax, eax
0044ab7b  je         0x44ab91

// block 0044ab7d
0044ab7d  lea        ecx, [ecx]

// block 0044ab80
0044ab80  mov        edi, dword ptr [eax]
0044ab82  cmp        dword ptr [edi + 0x27b4], ecx
0044ab88  je         0x44ab98

// block 0044ab8a
0044ab8a  mov        eax, dword ptr [eax + 4]
0044ab8d  test       eax, eax
0044ab8f  jne        0x44ab80

// block 0044ab91
0044ab91  fldz       
0044ab93  pop        edi
0044ab94  mov        esp, ebp
0044ab96  pop        ebp
0044ab97  ret        

// block 0044ab98
0044ab98  mov        eax, dword ptr [edx + 0x34]
0044ab9b  fld        dword ptr [esi]
0044ab9d  fsub       dword ptr [eax + 0x1074]
0044aba3  fstp       dword ptr [esp + 0xc]
0044aba7  fld        dword ptr [esi + 4]
0044abaa  fsub       dword ptr [eax + 0x1078]
0044abb0  fstp       dword ptr [esp + 8]
0044abb4  fldz       
0044abb6  fld        st(0)
0044abb8  fld        dword ptr [esp + 8]
0044abbc  fucom      st(1)
0044abbe  fnstsw     ax
0044abc0  fstp       st(1)
0044abc2  fld        dword ptr [esp + 0xc]
0044abc6  test       ah, 0x44
0044abc9  jp         0x44abe5

// block 0044abcb
0044abcb  fucom      st(2)
0044abcd  fnstsw     ax
0044abcf  fstp       st(2)
0044abd1  test       ah, 0x44
0044abd4  jp         0x44abe7

// block 0044abd6
0044abd6  fstp       st(0)
0044abd8  fstp       st(0)
0044abda  fld        dword ptr [0x4a3e08]
0044abe0  pop        edi
0044abe1  mov        esp, ebp
0044abe3  pop        ebp
0044abe4  ret        

// block 0044abe5
0044abe5  fstp       st(2)

// block 0044abe7
0044abe7  fxch       st(1)
0044abe9  call       0x4937aa

// block 0044abee
0044abee  fstp       dword ptr [esp + 0xc]
0044abf2  fld        dword ptr [esp + 0xc]
0044abf6  pop        edi
0044abf7  mov        esp, ebp
0044abf9  pop        ebp
0044abfa  ret        
