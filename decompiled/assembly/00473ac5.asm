
// block 00473ac5
00473ac5  mov        edi, edi
00473ac7  push       ebp
00473ac8  mov        ebp, esp
00473aca  sub        esp, 0x20
00473acd  mov        eax, dword ptr [0x4ad138]
00473ad2  xor        eax, ebp
00473ad4  mov        dword ptr [ebp - 4], eax
00473ad7  push       ebx
00473ad8  mov        ebx, dword ptr [ebp + 0xc]
00473adb  push       esi
00473adc  mov        esi, dword ptr [ebp + 8]
00473adf  push       edi
00473ae0  call       0x473a49

// block 00473ae5
00473ae5  mov        edi, eax
00473ae7  xor        esi, esi
00473ae9  mov        dword ptr [ebp + 8], edi
00473aec  cmp        edi, esi
00473aee  jne        0x473afe

// block 00473af0
00473af0  mov        eax, ebx
00473af2  call       0x4737ae

// block 00473af7
00473af7  xor        eax, eax
00473af9  jmp        0x473c9b

// block 00473afe
00473afe  mov        dword ptr [ebp - 0x1c], esi
00473b01  xor        eax, eax

// block 00473b03
00473b03  cmp        dword ptr [eax + 0x4ad8e0], edi
00473b09  je         0x473ba0

// block 00473b0f
00473b0f  inc        dword ptr [ebp - 0x1c]
00473b12  add        eax, 0x30
00473b15  cmp        eax, 0xf0
00473b1a  jb         0x473b03

// block 00473b1c
00473b1c  cmp        edi, 0xfde8
00473b22  je         0x473c98

// block 00473b28
00473b28  cmp        edi, 0xfde9
00473b2e  je         0x473c98

// block 00473b34
00473b34  movzx      eax, di
00473b37  push       eax
00473b38  call       dword ptr [0x498150]

// block 00473b3e
00473b3e  test       eax, eax
00473b40  je         0x473c98

// block 00473b46
00473b46  lea        eax, [ebp - 0x18]
00473b49  push       eax
00473b4a  push       edi
00473b4b  call       dword ptr [0x498164]

// block 00473b51
00473b51  test       eax, eax
00473b53  je         0x473c8c

// block 00473b59
00473b59  push       0x101
00473b5e  lea        eax, [ebx + 0x1c]
00473b61  push       esi
00473b62  push       eax
00473b63  call       0x477420

// block 00473b68
00473b68  xor        edx, edx
00473b6a  inc        edx
00473b6b  add        esp, 0xc
00473b6e  mov        dword ptr [ebx + 4], edi
00473b71  mov        dword ptr [ebx + 0xc], esi
00473b74  cmp        dword ptr [ebp - 0x18], edx
00473b77  jbe        0x473c75

// block 00473b7d
00473b7d  cmp        byte ptr [ebp - 0x12], 0
00473b81  je         0x473c56

// block 00473b87
00473b87  lea        esi, [ebp - 0x11]

// block 00473b8a
00473b8a  mov        cl, byte ptr [esi]
00473b8c  test       cl, cl
00473b8e  je         0x473c56

// block 00473b94
00473b94  movzx      eax, byte ptr [esi - 1]
00473b98  movzx      ecx, cl
00473b9b  jmp        0x473c46

// block 00473ba0
00473ba0  push       0x101
00473ba5  lea        eax, [ebx + 0x1c]
00473ba8  push       esi
00473ba9  push       eax
00473baa  call       0x477420

// block 00473baf
00473baf  mov        ecx, dword ptr [ebp - 0x1c]
00473bb2  add        esp, 0xc
00473bb5  imul       ecx, ecx, 0x30
00473bb8  mov        dword ptr [ebp - 0x20], esi
00473bbb  lea        esi, [ecx + 0x4ad8f0]
00473bc1  mov        dword ptr [ebp - 0x1c], esi
00473bc4  jmp        0x473bf0

// block 00473bc6
00473bc6  mov        al, byte ptr [esi + 1]
00473bc9  test       al, al
00473bcb  je         0x473bf5

// block 00473bcd
00473bcd  movzx      edi, byte ptr [esi]
00473bd0  movzx      eax, al
00473bd3  jmp        0x473be7

// block 00473bd5
00473bd5  mov        eax, dword ptr [ebp - 0x20]
00473bd8  mov        al, byte ptr [eax + 0x4ad8dc]
00473bde  or         byte ptr [ebx + edi + 0x1d], al
00473be2  movzx      eax, byte ptr [esi + 1]
00473be6  inc        edi

// block 00473be7
00473be7  cmp        edi, eax
00473be9  jbe        0x473bd5

// block 00473beb
00473beb  mov        edi, dword ptr [ebp + 8]
00473bee  inc        esi
00473bef  inc        esi

// block 00473bf0
00473bf0  cmp        byte ptr [esi], 0
00473bf3  jne        0x473bc6

// block 00473bf5
00473bf5  mov        esi, dword ptr [ebp - 0x1c]
00473bf8  inc        dword ptr [ebp - 0x20]
00473bfb  add        esi, 8
00473bfe  cmp        dword ptr [ebp - 0x20], 4
00473c02  mov        dword ptr [ebp - 0x1c], esi
00473c05  jb         0x473bf0

// block 00473c07
00473c07  mov        eax, edi
00473c09  mov        dword ptr [ebx + 4], edi
00473c0c  mov        dword ptr [ebx + 8], 1
00473c13  call       0x47377f

// block 00473c18
00473c18  push       6
00473c1a  mov        dword ptr [ebx + 0xc], eax
00473c1d  lea        eax, [ebx + 0x10]
00473c20  lea        ecx, [ecx + 0x4ad8e4]
00473c26  pop        edx

// block 00473c27
00473c27  mov        si, word ptr [ecx]
00473c2a  inc        ecx
00473c2b  mov        word ptr [eax], si
00473c2e  inc        ecx
00473c2f  inc        eax
00473c30  inc        eax
00473c31  dec        edx
00473c32  jne        0x473c27

// block 00473c34
00473c34  mov        esi, ebx
00473c36  call       0x473812

// block 00473c3b
00473c3b  jmp        0x473af7

// block 00473c40
00473c40  or         byte ptr [ebx + eax + 0x1d], 4
00473c45  inc        eax

// block 00473c46
00473c46  cmp        eax, ecx
00473c48  jbe        0x473c40

// block 00473c4a
00473c4a  inc        esi
00473c4b  inc        esi
00473c4c  cmp        byte ptr [esi - 1], 0
00473c50  jne        0x473b8a

// block 00473c56
00473c56  lea        eax, [ebx + 0x1e]
00473c59  mov        ecx, 0xfe

// block 00473c5e
00473c5e  or         byte ptr [eax], 8
00473c61  inc        eax
00473c62  dec        ecx
00473c63  jne        0x473c5e

// block 00473c65
00473c65  mov        eax, dword ptr [ebx + 4]
00473c68  call       0x47377f

// block 00473c6d
00473c6d  mov        dword ptr [ebx + 0xc], eax
00473c70  mov        dword ptr [ebx + 8], edx
00473c73  jmp        0x473c78

// block 00473c75
00473c75  mov        dword ptr [ebx + 8], esi

// block 00473c78
00473c78  xor        eax, eax
00473c7a  movzx      ecx, ax
00473c7d  mov        eax, ecx
00473c7f  shl        ecx, 0x10
00473c82  or         eax, ecx
00473c84  lea        edi, [ebx + 0x10]
00473c87  stosd      dword ptr es:[edi], eax
00473c88  stosd      dword ptr es:[edi], eax
00473c89  stosd      dword ptr es:[edi], eax
00473c8a  jmp        0x473c34

// block 00473c8c
00473c8c  cmp        dword ptr [0x4b40c0], esi
00473c92  jne        0x473af0

// block 00473c98
00473c98  or         eax, 0xffffffff

// block 00473c9b
00473c9b  mov        ecx, dword ptr [ebp - 4]
00473c9e  pop        edi
00473c9f  pop        esi
00473ca0  xor        ecx, ebp
00473ca2  pop        ebx
00473ca3  call       0x46c932

// block 00473ca8
00473ca8  leave      
00473ca9  ret        
