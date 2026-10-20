
// block 0042fca0
0042fca0  sub        esp, 0x10
0042fca3  cmp        dword ptr [0x4cefb8], 0
0042fcaa  push       ebp
0042fcab  push       esi
0042fcac  push       edi
0042fcad  mov        esi, eax
0042fcaf  je         0x42fcc4

// block 0042fcb1
0042fcb1  mov        edi, dword ptr [0x498084]

// block 0042fcb7
0042fcb7  push       0xa
0042fcb9  call       edi

// block 0042fcbb
0042fcbb  cmp        dword ptr [0x4cefb8], 0
0042fcc2  jne        0x42fcb7

// block 0042fcc4
0042fcc4  push       esi
0042fcc5  push       0x4a0a78
0042fcca  mov        dword ptr [esp + 0x14], 0
0042fcd2  call       0x4319e0

// block 0042fcd7
0042fcd7  mov        eax, dword ptr [0x4ce8f0]
0042fcdc  mov        ecx, dword ptr [eax]
0042fcde  add        esp, 8
0042fce1  lea        edx, [esp + 0xc]
0042fce5  push       edx
0042fce6  push       0
0042fce8  push       0
0042fcea  push       0
0042fcec  push       eax
0042fced  mov        eax, dword ptr [ecx + 0x48]
0042fcf0  call       eax

// block 0042fcf2
0042fcf2  xor        eax, eax
0042fcf4  mov        dword ptr [0x4cefbc], eax
0042fcf9  mov        dword ptr [0x4cefc0], eax
0042fcfe  mov        dword ptr [0x4cefc4], eax
0042fd03  mov        word ptr [0x4cefc8], ax
0042fd09  mov        eax, 0x36
0042fd0e  mov        ecx, 0x4d42
0042fd13  mov        edx, 0x4cefd4
0042fd18  mov        dword ptr [0x4cefc6], eax
0042fd1d  mov        dword ptr [0x4cefbe], eax
0042fd22  mov        word ptr [0x4cefbc], cx
0042fd29  mov        eax, esi
0042fd2b  sub        edx, esi
0042fd2d  lea        ecx, [ecx]

// block 0042fd30
0042fd30  mov        cl, byte ptr [eax]
0042fd32  mov        byte ptr [edx + eax], cl
0042fd35  inc        eax
0042fd36  test       cl, cl
0042fd38  jne        0x42fd30

// block 0042fd3a
0042fd3a  mov        eax, dword ptr [0x4ce9e4]
0042fd3f  sub        eax, 0x16
0042fd42  je         0x42fd79

// block 0042fd44
0042fd44  sub        eax, 1
0042fd47  mov        ecx, 0x4b0ec8
0042fd4c  je         0x42fd67

// block 0042fd4e
0042fd4e  push       0x4a0b0c
0042fd53  call       0x464220

// block 0042fd58
0042fd58  add        esp, 4
0042fd5b  mov        eax, 1
0042fd60  pop        edi
0042fd61  pop        esi
0042fd62  pop        ebp
0042fd63  add        esp, 0x10
0042fd66  ret        

// block 0042fd67
0042fd67  push       0x4a0a8c
0042fd6c  call       0x464220

// block 0042fd71
0042fd71  add        esp, 4
0042fd74  jmp        0x42fe93

// block 0042fd79
0042fd79  push       0x2c
0042fd7b  call       0x46d04a

// block 0042fd80
0042fd80  add        esp, 4
0042fd83  mov        dword ptr [0x4cefcc], eax
0042fd88  test       eax, eax
0042fd8a  je         0x42fdac

// block 0042fd8c
0042fd8c  push       0x2c
0042fd8e  push       0
0042fd90  push       eax
0042fd91  call       0x477420

// block 0042fd96
0042fd96  push       0xe1000
0042fd9b  call       0x46d04a

// block 0042fda0
0042fda0  add        esp, 0x10
0042fda3  mov        dword ptr [0x4cefd0], eax
0042fda8  test       eax, eax
0042fdaa  jne        0x42fdc3

// block 0042fdac
0042fdac  push       0x4a0acc
0042fdb1  mov        ecx, 0x4b0ec8
0042fdb6  call       0x464220

// block 0042fdbb
0042fdbb  add        esp, 4
0042fdbe  jmp        0x42fe93

// block 0042fdc3
0042fdc3  mov        eax, dword ptr [0x4cefcc]
0042fdc8  add        dword ptr [0x4cefbe], 0xe1000
0042fdd2  mov        edx, 0x18
0042fdd7  mov        word ptr [eax + 0xe], dx
0042fddb  mov        ecx, dword ptr [0x4cefcc]
0042fde1  mov        dword ptr [ecx], 0x28
0042fde7  mov        edx, dword ptr [0x4cefcc]
0042fded  mov        dword ptr [edx + 4], 0x280
0042fdf4  mov        eax, dword ptr [0x4cefcc]
0042fdf9  mov        dword ptr [eax + 8], 0x1e0
0042fe00  mov        edx, dword ptr [0x4cefcc]
0042fe06  mov        ecx, 1
0042fe0b  mov        word ptr [edx + 0xc], cx
0042fe0f  mov        eax, dword ptr [0x4cefcc]
0042fe14  push       0
0042fe16  mov        dword ptr [eax + 0x10], 0
0042fe1d  mov        eax, dword ptr [esp + 0x10]
0042fe21  mov        ecx, dword ptr [eax]
0042fe23  push       0
0042fe25  lea        edx, [esp + 0x18]
0042fe29  push       edx
0042fe2a  push       eax
0042fe2b  mov        eax, dword ptr [ecx + 0x34]
0042fe2e  call       eax

// block 0042fe30
0042fe30  mov        edi, 0x1df
0042fe35  xor        ebp, ebp

// block 0042fe37
0042fe37  mov        eax, dword ptr [esp + 0x10]
0042fe3b  mov        ecx, dword ptr [0x4cefd0]
0042fe41  imul       eax, edi
0042fe44  add        ecx, ebp
0042fe46  add        eax, dword ptr [esp + 0x14]
0042fe4a  mov        esi, 0x280
0042fe4f  nop        

// block 0042fe50
0042fe50  mov        dx, word ptr [eax]
0042fe53  mov        word ptr [ecx], dx
0042fe56  mov        dl, byte ptr [eax + 2]
0042fe59  mov        byte ptr [ecx + 2], dl
0042fe5c  add        eax, 4
0042fe5f  add        ecx, 3
0042fe62  sub        esi, 1
0042fe65  jne        0x42fe50

// block 0042fe67
0042fe67  dec        edi
0042fe68  add        ebp, 0x780
0042fe6e  cmp        edi, -1
0042fe71  jg         0x42fe37

// block 0042fe73
0042fe73  mov        eax, dword ptr [esp + 0xc]
0042fe77  mov        ecx, dword ptr [eax]
0042fe79  mov        edx, dword ptr [ecx + 0x38]
0042fe7c  push       eax
0042fe7d  call       edx

// block 0042fe7f
0042fe7f  push       esi
0042fe80  push       esi
0042fe81  push       0x42fbb0
0042fe86  call       0x46db4a

// block 0042fe8b
0042fe8b  add        esp, 0xc
0042fe8e  mov        dword ptr [0x4cefb8], eax

// block 0042fe93
0042fe93  mov        eax, dword ptr [esp + 0xc]
0042fe97  test       eax, eax
0042fe99  je         0x42fea3

// block 0042fe9b
0042fe9b  mov        ecx, dword ptr [eax]
0042fe9d  mov        edx, dword ptr [ecx + 8]
0042fea0  push       eax
0042fea1  call       edx

// block 0042fea3
0042fea3  pop        edi
0042fea4  pop        esi
0042fea5  xor        eax, eax
0042fea7  pop        ebp
0042fea8  add        esp, 0x10
0042feab  ret        
