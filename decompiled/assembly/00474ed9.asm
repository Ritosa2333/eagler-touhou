
// block 00474ed9
00474ed9  mov        edi, edi
00474edb  push       ebp
00474edc  mov        ebp, esp
00474ede  cmp        dword ptr [ebp + 8], 5
00474ee2  push       esi
00474ee3  push       edi
00474ee4  ja         0x474f0a

// block 00474ee6
00474ee6  cmp        dword ptr [ebp + 0xc], 0
00474eea  je         0x474f0a

// block 00474eec
00474eec  xor        edi, edi
00474eee  inc        edi
00474eef  push       edi
00474ef0  push       8
00474ef2  call       0x477813

// block 00474ef7
00474ef7  mov        esi, eax
00474ef9  pop        ecx
00474efa  pop        ecx
00474efb  test       esi, esi
00474efd  jne        0x474f10

// block 00474eff
00474eff  call       0x46e96f

// block 00474f04
00474f04  mov        dword ptr [eax], 0xc

// block 00474f0a
00474f0a  xor        eax, eax

// block 00474f0c
00474f0c  pop        edi
00474f0d  pop        esi
00474f0e  pop        ebp
00474f0f  ret        

// block 00474f10
00474f10  push       edi
00474f11  push       0xd8
00474f16  call       0x477813

// block 00474f1b
00474f1b  pop        ecx
00474f1c  pop        ecx
00474f1d  mov        dword ptr [esi], eax
00474f1f  test       eax, eax
00474f21  jne        0x474f2c

// block 00474f23
00474f23  push       esi
00474f24  call       0x46c941

// block 00474f29
00474f29  pop        ecx
00474f2a  jmp        0x474eff

// block 00474f2c
00474f2c  push       edi
00474f2d  push       0x220
00474f32  call       0x477813

// block 00474f37
00474f37  pop        ecx
00474f38  pop        ecx
00474f39  mov        dword ptr [esi + 4], eax
00474f3c  test       eax, eax
00474f3e  jne        0x474f50

// block 00474f40
00474f40  push       dword ptr [esi]
00474f42  call       0x46c941

// block 00474f47
00474f47  push       esi
00474f48  call       0x46c941

// block 00474f4d
00474f4d  pop        ecx
00474f4e  jmp        0x474f29

// block 00474f50
00474f50  mov        eax, dword ptr [esi]
00474f52  mov        ecx, 0x4ad9e0
00474f57  call       0x47411d

// block 00474f5c
00474f5c  push       dword ptr [ebp + 8]
00474f5f  mov        ecx, dword ptr [ebp + 0xc]
00474f62  mov        edx, dword ptr [esi]
00474f64  call       0x474cbe

// block 00474f69
00474f69  pop        ecx
00474f6a  test       eax, eax
00474f6c  jne        0x474f87

// block 00474f6e
00474f6e  push       dword ptr [esi]
00474f70  call       0x474084

// block 00474f75
00474f75  push       dword ptr [esi]
00474f77  call       0x473eac

// block 00474f7c
00474f7c  push       esi
00474f7d  call       0x46c941

// block 00474f82
00474f82  add        esp, 0xc
00474f85  jmp        0x474fb9

// block 00474f87
00474f87  push       dword ptr [esi + 4]
00474f8a  mov        eax, dword ptr [esi]
00474f8c  push       dword ptr [eax + 4]
00474f8f  call       0x473ac5

// block 00474f94
00474f94  pop        ecx
00474f95  pop        ecx
00474f96  test       eax, eax
00474f98  je         0x474fbd

// block 00474f9a
00474f9a  push       dword ptr [esi + 4]
00474f9d  call       0x46c941

// block 00474fa2
00474fa2  push       dword ptr [esi]
00474fa4  call       0x474084

// block 00474fa9
00474fa9  push       dword ptr [esi]
00474fab  call       0x473eac

// block 00474fb0
00474fb0  push       esi
00474fb1  call       0x46c941

// block 00474fb6
00474fb6  add        esp, 0x10

// block 00474fb9
00474fb9  xor        esi, esi
00474fbb  jmp        0x474fc7

// block 00474fbd
00474fbd  mov        eax, dword ptr [esi + 4]
00474fc0  mov        dword ptr [eax], edi
00474fc2  mov        eax, dword ptr [esi + 4]
00474fc5  mov        dword ptr [eax], edi

// block 00474fc7
00474fc7  mov        eax, esi
00474fc9  jmp        0x474f0c
