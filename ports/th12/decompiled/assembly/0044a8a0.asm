
// block 0044a8a0
0044a8a0  push       ebp
0044a8a1  mov        ebp, esp
0044a8a3  and        esp, 0xfffffff8
0044a8a6  sub        esp, 0x60
0044a8a9  push       ebx
0044a8aa  xor        ebx, ebx
0044a8ac  push       edi
0044a8ad  mov        edi, eax
0044a8af  cmp        dword ptr [esi + 0x2c], ebx
0044a8b2  jne        0x44ab59

// block 0044a8b8
0044a8b8  push       0x50
0044a8ba  lea        eax, [esp + 0x1c]
0044a8be  push       ebx
0044a8bf  push       eax
0044a8c0  mov        dword ptr [esi + 0x2c], 1
0044a8c7  call       0x477420

// block 0044a8cc
0044a8cc  fldz       
0044a8ce  mov        eax, dword ptr [0x4b0c48]
0044a8d3  fstp       dword ptr [esp + 0x24]
0044a8d7  cdq        
0044a8d8  fld        dword ptr [0x4a3db8]
0044a8de  idiv       dword ptr [0x4b0cd4]
0044a8e4  fstp       dword ptr [esp + 0x28]
0044a8e8  add        esp, 0xc
0044a8eb  mov        dword ptr [esp + 0x28], ebx
0044a8ef  mov        dword ptr [esp + 0x24], ebx
0044a8f3  mov        ecx, dword ptr [eax*4 + 0x4b33f0]
0044a8fa  mov        dword ptr [esp + 0x2c], ecx
0044a8fe  lea        eax, [ebx + 3]
0044a901  cmp        edi, -1
0044a904  je         0x44a909

// block 0044a906
0044a906  lea        eax, [edi - 1]

// block 0044a909
0044a909  mov        eax, dword ptr [eax*4 + 0x4b33e0]
0044a910  lea        edx, [esp + 0x18]
0044a914  push       edx
0044a915  push       eax
0044a916  call       0x412990

// block 0044a91b
0044a91b  mov        edx, dword ptr [0x4b43e4]
0044a921  mov        dword ptr [esi + 0x34], eax
0044a924  mov        ecx, dword ptr [eax + 0x27b4]
0044a92a  push       -1
0044a92c  mov        dword ptr [esi + 0x38], ecx
0044a92f  mov        dword ptr [eax + 0x103c], 0x44a890
0044a939  push       edx
0044a93a  mov        dword ptr [esi + 0x30], edi
0044a93d  call       0x4214b0

// block 0044a942
0044a942  lea        eax, [esp + 0x18]
0044a946  push       eax
0044a947  push       2
0044a949  lea        ecx, [esp + 0x10]
0044a94d  push       ecx
0044a94e  call       0x40fbe0

// block 0044a953
0044a953  mov        eax, dword ptr [0x4b43e4]
0044a958  mov        ecx, dword ptr [eax + 0x6d44]
0044a95e  push       ebx
0044a95f  push       0x52
0044a961  lea        edx, [esp + 0x10]
0044a965  push       edx
0044a966  push       ecx
0044a967  mov        eax, 0x17
0044a96c  xor        ecx, ecx
0044a96e  call       0x4615a0

// block 0044a973
0044a973  mov        edx, dword ptr [esp + 8]
0044a977  push       edx
0044a978  mov        edx, dword ptr [0x4ce8cc]
0044a97e  call       0x461920

// block 0044a983
0044a983  mov        ebx, eax
0044a985  cmp        edi, -1
0044a988  jne        0x44a991

// block 0044a98a
0044a98a  mov        edi, 3
0044a98f  jmp        0x44a992

// block 0044a991
0044a991  dec        edi

// block 0044a992
0044a992  mov        eax, dword ptr [ebx + 0x494]
0044a998  add        edi, 7
0044a99b  movzx      edi, di
0044a99e  test       eax, eax
0044a9a0  je         0x44a9a9

// block 0044a9a2
0044a9a2  movsx      edx, di
0044a9a5  mov        ecx, ebx
0044a9a7  call       eax

// block 0044a9a9
0044a9a9  mov        word ptr [ebx + 0x3c4], di
0044a9b0  mov        eax, dword ptr [esi + 0x20]
0044a9b3  test       al, 1
0044a9b5  jne        0x44a9d7

// block 0044a9b7
0044a9b7  fldz       
0044a9b9  or         eax, 1
0044a9bc  fstp       dword ptr [esi + 0x18]
0044a9bf  mov        dword ptr [esi + 0x14], 0
0044a9c6  mov        dword ptr [esi + 0x10], 0xfff0bdc1
0044a9cd  mov        dword ptr [esi + 0x1c], 0x4b2ed0
0044a9d4  mov        dword ptr [esi + 0x20], eax

// block 0044a9d7
0044a9d7  fldz       
0044a9d9  mov        edx, 0x34
0044a9de  fstp       dword ptr [esi + 0x18]
0044a9e1  mov        dword ptr [esi + 0x14], 0
0044a9e8  mov        dword ptr [esi + 0x10], 0xffffffff
0044a9ef  call       0x453d90

// block 0044a9f4
0044a9f4  mov        eax, dword ptr [esi + 0x34]
0044a9f7  fld        dword ptr [eax + 0x1074]
0044a9fd  push       ecx
0044a9fe  mov        eax, 0x36
0044aa03  fstp       dword ptr [esp]
0044aa06  call       0x453e20

// block 0044aa0b
0044aa0b  mov        edi, dword ptr [esi + 0x34]
0044aa0e  mov        ecx, dword ptr [0x4b43c8]
0044aa14  mov        ebx, dword ptr [ecx + 0x4debdc]
0044aa1a  add        edi, 0x1074
0044aa20  test       dword ptr [0x4cee78], 0x8000
0044aa2a  mov        dword ptr [esp + 8], ebx
0044aa2e  je         0x44aa41

// block 0044aa30
0044aa30  push       0x4cf1d0
0044aa35  call       dword ptr [0x498088]

// block 0044aa3b
0044aa3b  inc        byte ptr [0x4cf221]

// block 0044aa41
0044aa41  inc        dword ptr [ebx + 0x130]
0044aa47  call       0x4621c0

// block 0044aa4c
0044aa4c  mov        ebx, eax
0044aa4e  or         dword ptr [ebx + 0x480], 1
0044aa55  mov        dword ptr [ebx + 0x20], 0x17
0044aa5c  test       edi, edi
0044aa5e  jne        0x44aa8e

// block 0044aa60
0044aa60  fldz       
0044aa62  fst        dword ptr [esp + 0xc]
0044aa66  mov        edx, dword ptr [esp + 0xc]
0044aa6a  fst        dword ptr [esp + 0x10]
0044aa6e  mov        eax, dword ptr [esp + 0x10]
0044aa72  fstp       dword ptr [esp + 0x14]
0044aa76  mov        ecx, dword ptr [esp + 0x14]
0044aa7a  mov        dword ptr [ebx + 0x430], edx
0044aa80  mov        dword ptr [ebx + 0x434], eax
0044aa86  mov        dword ptr [ebx + 0x438], ecx
0044aa8c  jmp        0x44aaba

// block 0044aa8e
0044aa8e  fld        dword ptr [edi]
0044aa90  fadd       qword ptr [0x4a3d70]
0044aa96  fadd       qword ptr [0x4a3d28]
0044aa9c  fstp       dword ptr [ebx + 0x430]
0044aaa2  fld        dword ptr [edi + 4]
0044aaa5  fadd       qword ptr [0x4a3d68]
0044aaab  fstp       dword ptr [ebx + 0x434]
0044aab1  fld        dword ptr [edi + 8]
0044aab4  fstp       dword ptr [ebx + 0x438]

// block 0044aaba
0044aaba  mov        ecx, dword ptr [esp + 8]
0044aabe  push       0xcf
0044aac3  push       ebx
0044aac4  call       0x454d10

// block 0044aac9
0044aac9  lea        eax, [esp + 8]
0044aacd  call       0x461250

// block 0044aad2
0044aad2  test       dword ptr [0x4cee78], 0x8000
0044aadc  mov        edi, dword ptr [eax]
0044aade  je         0x44aaf1

// block 0044aae0
0044aae0  push       0x4cf1d0
0044aae5  call       dword ptr [0x49808c]

// block 0044aaeb
0044aaeb  dec        byte ptr [0x4cf221]

// block 0044aaf1
0044aaf1  fldz       
0044aaf3  mov        eax, dword ptr [0x4b43e4]
0044aaf8  mov        dword ptr [esi + 0x3c], edi
0044aafb  fstp       dword ptr [esi + 0x50]
0044aafe  xor        edi, edi
0044ab00  push       edi
0044ab01  push       0x86
0044ab06  lea        edx, [esp + 0x10]
0044ab0a  mov        dword ptr [esi + 0x48], edi
0044ab0d  mov        dword ptr [esi + 0x4c], edi
0044ab10  mov        ecx, dword ptr [eax + 0x6d44]
0044ab16  push       edx
0044ab17  push       ecx
0044ab18  lea        eax, [edi + 0x17]
0044ab1b  xor        ecx, ecx
0044ab1d  call       0x4615a0

// block 0044ab22
0044ab22  mov        edx, dword ptr [esp + 8]
0044ab26  mov        ecx, dword ptr [0x4b43e4]
0044ab2c  push       edi
0044ab2d  push       0x88
0044ab32  lea        eax, [esp + 0x10]
0044ab36  mov        dword ptr [esi + 0x40], edx
0044ab39  mov        edx, dword ptr [ecx + 0x6d44]
0044ab3f  push       eax
0044ab40  push       edx
0044ab41  lea        eax, [edi + 0x17]
0044ab44  xor        ecx, ecx
0044ab46  call       0x4615a0

// block 0044ab4b
0044ab4b  mov        eax, dword ptr [esp + 8]
0044ab4f  mov        dword ptr [esi + 0x44], eax
0044ab52  mov        dword ptr [esi + 0x28], 0x2d0

// block 0044ab59
0044ab59  pop        edi
0044ab5a  pop        ebx
0044ab5b  mov        esp, ebp
0044ab5d  pop        ebp
0044ab5e  ret        
