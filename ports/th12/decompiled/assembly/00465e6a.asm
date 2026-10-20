
// block 00465e6a
00465e6a  push       edx
00465e6b  push       ebp
00465e6c  push       eax
00465e6d  mov        eax, dword ptr [ecx + 0xc]
00465e70  call       eax

// block 00465e72
00465e72  test       eax, eax
00465e74  jl         0x465f37

// block 00465e7a
00465e7a  mov        ecx, dword ptr [esi + 4]
00465e7d  mov        edi, dword ptr [edi + ecx]
00465e80  mov        edx, dword ptr [edi]
00465e82  mov        ecx, dword ptr [edx]
00465e84  lea        eax, [esp + 0xc]
00465e88  push       eax
00465e89  push       0x49981c
00465e8e  push       edi
00465e8f  call       ecx

// block 00465e91
00465e91  test       eax, eax
00465e93  jl         0x465f37

// block 00465e99
00465e99  push       0x80
00465e9e  call       0x46c9ea

// block 00465ea3
00465ea3  mov        edi, eax
00465ea5  add        esp, 4
00465ea8  test       edi, edi
00465eaa  je         0x465f10

// block 00465eac
00465eac  xor        eax, eax
00465eae  mov        edi, edi

// block 00465eb0
00465eb0  mov        edx, dword ptr [esi + 0x70]
00465eb3  lea        ecx, [eax + 1]
00465eb6  imul       edx, ecx
00465eb9  dec        edx
00465eba  mov        dword ptr [edi + eax*8], edx
00465ebd  mov        edx, dword ptr [esi + 0x74]
00465ec0  mov        dword ptr [edi + eax*8 + 4], edx
00465ec4  mov        eax, ecx
00465ec6  cmp        eax, 0x10
00465ec9  jb         0x465eb0

// block 00465ecb
00465ecb  mov        eax, dword ptr [esp + 0xc]
00465ecf  mov        ecx, dword ptr [eax]
00465ed1  mov        edx, dword ptr [ecx + 0xc]
00465ed4  push       edi
00465ed5  push       0x10
00465ed7  push       eax
00465ed8  call       edx

// block 00465eda
00465eda  test       eax, eax
00465edc  mov        eax, dword ptr [esp + 0xc]
00465ee0  jl         0x465f1a

// block 00465ee2
00465ee2  test       eax, eax
00465ee4  je         0x465ef6

// block 00465ee6
00465ee6  mov        ecx, dword ptr [eax]
00465ee8  mov        edx, dword ptr [ecx + 8]
00465eeb  push       eax
00465eec  call       edx

// block 00465eee
00465eee  mov        dword ptr [esp + 0xc], 0

// block 00465ef6
00465ef6  push       edi
00465ef7  call       0x46ca4f

// block 00465efc
00465efc  inc        ebx
00465efd  add        esp, 4
00465f00  cmp        ebx, dword ptr [esi + 0x10]
00465f03  jb         0x465e55

// block 00465f10
00465f10  pop        edi
00465f11  pop        ebp
00465f12  mov        eax, 0x8007000e
00465f17  pop        ebx
00465f18  pop        ecx
00465f19  ret        

// block 00465f1a
00465f1a  test       eax, eax
00465f1c  je         0x465f2e

// block 00465f1e
00465f1e  mov        ecx, dword ptr [eax]
00465f20  mov        edx, dword ptr [ecx + 8]
00465f23  push       eax
00465f24  call       edx

// block 00465f26
00465f26  mov        dword ptr [esp + 0xc], 0

// block 00465f2e
00465f2e  push       edi
00465f2f  call       0x46ca4f

// block 00465f34
00465f34  add        esp, 4

// block 00465f37
00465f37  pop        edi
00465f38  pop        ebp
00465f39  mov        eax, 0x80004005
00465f3e  pop        ebx
00465f3f  pop        ecx
00465f40  ret        
