
// block 0046fd80
0046fd80  mov        edi, edi
0046fd82  push       ebp
0046fd83  mov        ebp, esp
0046fd85  sub        esp, 0x18
0046fd88  push       ebx
0046fd89  mov        ebx, dword ptr [ebp + 0xc]
0046fd8c  push       esi
0046fd8d  mov        esi, dword ptr [ebx + 8]
0046fd90  xor        esi, dword ptr [0x4ad138]
0046fd96  push       edi
0046fd97  mov        eax, dword ptr [esi]
0046fd99  mov        byte ptr [ebp - 1], 0
0046fd9d  mov        dword ptr [ebp - 0xc], 1
0046fda4  lea        edi, [ebx + 0x10]
0046fda7  cmp        eax, -2
0046fdaa  je         0x46fdb9

// block 0046fdac
0046fdac  mov        ecx, dword ptr [esi + 4]
0046fdaf  add        ecx, edi
0046fdb1  xor        ecx, dword ptr [eax + edi]
0046fdb4  call       0x46c932

// block 0046fdb9
0046fdb9  mov        ecx, dword ptr [esi + 0xc]
0046fdbc  mov        eax, dword ptr [esi + 8]
0046fdbf  add        ecx, edi
0046fdc1  xor        ecx, dword ptr [eax + edi]
0046fdc4  call       0x46c932

// block 0046fdc9
0046fdc9  mov        eax, dword ptr [ebp + 8]
0046fdcc  test       byte ptr [eax + 4], 0x66
0046fdd0  jne        0x46feec

// block 0046fdd6
0046fdd6  mov        ecx, dword ptr [ebp + 0x10]
0046fdd9  lea        edx, [ebp - 0x18]
0046fddc  mov        dword ptr [ebx - 4], edx
0046fddf  mov        ebx, dword ptr [ebx + 0xc]
0046fde2  mov        dword ptr [ebp - 0x18], eax
0046fde5  mov        dword ptr [ebp - 0x14], ecx
0046fde8  cmp        ebx, -2
0046fdeb  je         0x46fe4c

// block 0046fded
0046fded  lea        ecx, [ecx]

// block 0046fdf0
0046fdf0  lea        eax, [ebx + ebx*2]
0046fdf3  mov        ecx, dword ptr [esi + eax*4 + 0x14]
0046fdf7  lea        eax, [esi + eax*4 + 0x10]
0046fdfb  mov        dword ptr [ebp - 0x10], eax
0046fdfe  mov        eax, dword ptr [eax]
0046fe00  mov        dword ptr [ebp - 8], eax
0046fe03  test       ecx, ecx
0046fe05  je         0x46fe1b

// block 0046fe07
0046fe07  mov        edx, edi
0046fe09  call       0x47b56a

// block 0046fe0e
0046fe0e  mov        byte ptr [ebp - 1], 1
0046fe12  test       eax, eax
0046fe14  jl         0x46fe56

// block 0046fe16
0046fe16  jg         0x46fe5f

// block 0046fe18
0046fe18  mov        eax, dword ptr [ebp - 8]

// block 0046fe1b
0046fe1b  mov        ebx, eax
0046fe1d  cmp        eax, -2
0046fe20  jne        0x46fdf0

// block 0046fe22
0046fe22  cmp        byte ptr [ebp - 1], 0
0046fe26  je         0x46fe4c

// block 0046fe28
0046fe28  mov        eax, dword ptr [esi]
0046fe2a  cmp        eax, -2
0046fe2d  je         0x46fe3c

// block 0046fe2f
0046fe2f  mov        ecx, dword ptr [esi + 4]
0046fe32  add        ecx, edi
0046fe34  xor        ecx, dword ptr [eax + edi]
0046fe37  call       0x46c932

// block 0046fe3c
0046fe3c  mov        ecx, dword ptr [esi + 0xc]
0046fe3f  mov        edx, dword ptr [esi + 8]
0046fe42  add        ecx, edi
0046fe44  xor        ecx, dword ptr [edx + edi]
0046fe47  call       0x46c932

// block 0046fe4c
0046fe4c  mov        eax, dword ptr [ebp - 0xc]
0046fe4f  pop        edi
0046fe50  pop        esi
0046fe51  pop        ebx
0046fe52  mov        esp, ebp
0046fe54  pop        ebp
0046fe55  ret        

// block 0046fe56
0046fe56  mov        dword ptr [ebp - 0xc], 0
0046fe5d  jmp        0x46fe28

// block 0046fe5f
0046fe5f  mov        ecx, dword ptr [ebp + 8]
0046fe62  cmp        dword ptr [ecx], 0xe06d7363
0046fe68  jne        0x46fe93

// block 0046fe6a
0046fe6a  cmp        dword ptr [0x49f3f0], 0
0046fe71  je         0x46fe93

// block 0046fe73
0046fe73  push       0x49f3f0
0046fe78  call       0x477a40

// block 0046fe7d
0046fe7d  add        esp, 4
0046fe80  test       eax, eax
0046fe82  je         0x46fe93

// block 0046fe84
0046fe84  mov        edx, dword ptr [ebp + 8]
0046fe87  push       1
0046fe89  push       edx
0046fe8a  call       dword ptr [0x49f3f0]

// block 0046fe90
0046fe90  add        esp, 8

// block 0046fe93
0046fe93  mov        ecx, dword ptr [ebp + 0xc]
0046fe96  call       0x47b59a

// block 0046fe9b
0046fe9b  mov        eax, dword ptr [ebp + 0xc]
0046fe9e  cmp        dword ptr [eax + 0xc], ebx
0046fea1  je         0x46feb5

// block 0046fea3
0046fea3  push       0x4ad138
0046fea8  push       edi
0046fea9  mov        edx, ebx
0046feab  mov        ecx, eax
0046fead  call       0x47b5b4

// block 0046feb2
0046feb2  mov        eax, dword ptr [ebp + 0xc]

// block 0046feb5
0046feb5  mov        ecx, dword ptr [ebp - 8]
0046feb8  mov        dword ptr [eax + 0xc], ecx
0046febb  mov        eax, dword ptr [esi]
0046febd  cmp        eax, -2
0046fec0  je         0x46fecf

// block 0046fec2
0046fec2  mov        ecx, dword ptr [esi + 4]
0046fec5  add        ecx, edi
0046fec7  xor        ecx, dword ptr [eax + edi]
0046feca  call       0x46c932

// block 0046fecf
0046fecf  mov        ecx, dword ptr [esi + 0xc]
0046fed2  mov        edx, dword ptr [esi + 8]
0046fed5  add        ecx, edi
0046fed7  xor        ecx, dword ptr [edx + edi]
0046feda  call       0x46c932

// block 0046fedf
0046fedf  mov        eax, dword ptr [ebp - 0x10]
0046fee2  mov        ecx, dword ptr [eax + 8]
0046fee5  mov        edx, edi
0046fee7  call       0x47b581

// block 0046feec
0046feec  mov        edx, 0xfffffffe
0046fef1  cmp        dword ptr [ebx + 0xc], edx
0046fef4  je         0x46fe4c

// block 0046fefa
0046fefa  push       0x4ad138
0046feff  push       edi
0046ff00  mov        ecx, ebx
0046ff02  call       0x47b5b4

// block 0046ff07
0046ff07  jmp        0x46fe28
