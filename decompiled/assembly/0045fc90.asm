
// block 0045fc90
0045fc90  sub        esp, 0x110
0045fc96  mov        eax, dword ptr [0x4ad138]
0045fc9b  xor        eax, esp
0045fc9d  mov        dword ptr [esp + 0x10c], eax
0045fca4  mov        eax, dword ptr [esp + 0x114]
0045fcab  push       ebp
0045fcac  mov        ebp, dword ptr [esp + 0x11c]
0045fcb3  push       edi
0045fcb4  mov        edi, ecx
0045fcb6  push       edi
0045fcb7  push       0x4a32b0
0045fcbc  mov        dword ptr [esp + 0x10], eax
0045fcc0  call       0x4622e0

// block 0045fcc5
0045fcc5  add        esp, 8
0045fcc8  cmp        ebp, 0x20
0045fccb  jl         0x45fce6

// block 0045fccd
0045fccd  push       0x4a32c4
0045fcd2  mov        edi, 0x4b0ec8
0045fcd7  call       0x464300

// block 0045fcdc
0045fcdc  add        esp, 4
0045fcdf  xor        eax, eax
0045fce1  jmp        0x45fe3d

// block 0045fce6
0045fce6  push       ebx
0045fce7  push       esi
0045fce8  push       edi
0045fce9  lea        ecx, [esp + 0x1c]
0045fced  push       0x4a0f40
0045fcf2  push       ecx
0045fcf3  call       0x46cbbb

// block 0045fcf8
0045fcf8  add        esp, 0xc
0045fcfb  push       0
0045fcfd  push       0
0045fcff  lea        eax, [esp + 0x20]
0045fd03  call       0x463c10

// block 0045fd08
0045fd08  push       0x138
0045fd0d  mov        ebx, eax
0045fd0f  mov        dword ptr [esp + 0x18], 0
0045fd17  call       0x46c9ea

// block 0045fd1c
0045fd1c  mov        esi, eax
0045fd1e  add        esp, 4
0045fd21  test       esi, esi
0045fd23  je         0x45fd37

// block 0045fd25
0045fd25  push       0x138
0045fd2a  push       0
0045fd2c  push       esi
0045fd2d  call       0x477420

// block 0045fd32
0045fd32  add        esp, 0xc
0045fd35  jmp        0x45fd39

// block 0045fd37
0045fd37  xor        esi, esi

// block 0045fd39
0045fd39  mov        edx, dword ptr [esp + 0x10]
0045fd3d  mov        dword ptr [edx + ebp*4 + 0x4b50c0], esi
0045fd44  test       ebx, ebx
0045fd46  je         0x45fe35

// block 0045fd4c
0045fd4c  lea        edx, [esi + 4]
0045fd4f  mov        dword ptr [esi], ebp
0045fd51  mov        dword ptr [esi + 0x108], ebx
0045fd57  mov        eax, edi
0045fd59  sub        edx, edi
0045fd5b  jmp        0x45fd60

// block 0045fd60
0045fd60  mov        cl, byte ptr [eax]
0045fd62  mov        byte ptr [edx + eax], cl
0045fd65  inc        eax
0045fd66  test       cl, cl
0045fd68  jne        0x45fd60

// block 0045fd6a
0045fd6a  movzx      ebp, word ptr [ebx + 6]
0045fd6e  mov        edx, dword ptr [ebx + 0x24]
0045fd71  movzx      edi, word ptr [ebx + 4]
0045fd75  mov        eax, ebx
0045fd77  mov        ecx, 1
0045fd7c  mov        dword ptr [esp + 0x10], ebp
0045fd80  test       edx, edx
0045fd82  je         0x45fd9e

// block 0045fd84
0045fd84  add        eax, edx
0045fd86  movzx      edx, word ptr [eax + 6]
0045fd8a  add        ebp, edx
0045fd8c  movzx      edx, word ptr [eax + 4]
0045fd90  add        edi, edx
0045fd92  mov        edx, dword ptr [eax + 0x24]
0045fd95  inc        ecx
0045fd96  test       edx, edx
0045fd98  jne        0x45fd84

// block 0045fd9a
0045fd9a  mov        dword ptr [esp + 0x10], ebp

// block 0045fd9e
0045fd9e  lea        ebp, [ecx + ecx*4]
0045fda1  add        ebp, ebp
0045fda3  add        ebp, ebp
0045fda5  push       ebp
0045fda6  mov        dword ptr [esi + 0x10c], ecx
0045fdac  call       0x46d04a

// block 0045fdb1
0045fdb1  push       ebp
0045fdb2  push       0
0045fdb4  push       eax
0045fdb5  mov        dword ptr [esi + 0x120], eax
0045fdbb  call       0x477420

// block 0045fdc0
0045fdc0  lea        eax, [edi + edi*8]
0045fdc3  add        eax, eax
0045fdc5  add        eax, eax
0045fdc7  add        eax, eax
0045fdc9  push       eax
0045fdca  call       0x46d04a

// block 0045fdcf
0045fdcf  mov        ebp, dword ptr [esp + 0x24]
0045fdd3  lea        ecx, [ebp*4]
0045fdda  push       ecx
0045fddb  mov        dword ptr [esi + 0x118], eax
0045fde1  call       0x46d04a

// block 0045fde6
0045fde6  mov        dword ptr [esi + 0x114], edi
0045fdec  add        esp, 0x18
0045fdef  mov        dword ptr [esi + 0x11c], eax
0045fdf5  mov        dword ptr [esi + 0x110], ebp
0045fdfb  mov        edi, ebx
0045fdfd  lea        ecx, [ecx]

// block 0045fe00
0045fe00  test       edi, edi
0045fe02  je         0x45fe23

// block 0045fe04
0045fe04  mov        ebx, dword ptr [esp + 0x14]
0045fe08  push       edi
0045fe09  mov        ecx, esi
0045fe0b  call       0x45fee0

// block 0045fe10
0045fe10  test       eax, eax
0045fe12  jl         0x45fe35

// block 0045fe14
0045fe14  mov        eax, dword ptr [edi + 0x24]
0045fe17  inc        dword ptr [esp + 0x14]
0045fe1b  test       eax, eax
0045fe1d  je         0x45fe39

// block 0045fe1f
0045fe1f  add        edi, eax
0045fe21  jmp        0x45fe00

// block 0045fe23
0045fe23  push       0x4a33b8
0045fe28  mov        edi, 0x4b0ec8
0045fe2d  call       0x464300

// block 0045fe32
0045fe32  add        esp, 4

// block 0045fe35
0045fe35  xor        eax, eax
0045fe37  jmp        0x45fe3b

// block 0045fe39
0045fe39  mov        eax, esi

// block 0045fe3b
0045fe3b  pop        esi
0045fe3c  pop        ebx

// block 0045fe3d
0045fe3d  mov        ecx, dword ptr [esp + 0x114]
0045fe44  pop        edi
0045fe45  pop        ebp
0045fe46  xor        ecx, esp
0045fe48  call       0x46c932

// block 0045fe4d
0045fe4d  add        esp, 0x110
0045fe53  ret        8
