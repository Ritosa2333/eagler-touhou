
// block 004dc9d4
004dc9d4  sub        esp, 0x30c
004dc9da  push       ebx
004dc9db  mov        ebx, ecx
004dc9dd  push       ebp
004dc9de  push       esi
004dc9df  lea        ebp, [ebx + 4]
004dc9e2  push       edi
004dc9e3  push       1
004dc9e5  mov        ecx, ebp
004dc9e7  call       0x4dc615

// block 004dc9ec
004dc9ec  test       eax, eax
004dc9ee  jne        0x4dc9fe

// block 004dc9f0
004dc9f0  mov        edi, dword ptr [ebx + 0x260]
004dc9f6  mov        ecx, 0xbd

// block 004dc9fb
004dc9fb  rep stosd  dword ptr es:[edi], eax

// block 004dc9fd
004dc9fd  stosb      byte ptr es:[edi], al

// block 004dc9fe
004dc9fe  xor        esi, esi

// block 004dca00
004dca00  push       4
004dca02  mov        ecx, ebp
004dca04  call       0x4dc615

// block 004dca09
004dca09  mov        byte ptr [esp + esi + 0x10], al
004dca0d  inc        esi
004dca0e  cmp        esi, 0x13
004dca11  jb         0x4dca00

// block 004dca13
004dca13  lea        edi, [ebx + 0x1c0]
004dca19  lea        eax, [esp + 0x10]
004dca1d  push       eax
004dca1e  mov        ecx, edi
004dca20  call       0x4dc6a5

// block 004dca25
004dca25  test       al, al
004dca27  jne        0x4dca34

// block 004dca29
004dca29  pop        edi
004dca2a  pop        esi
004dca2b  pop        ebp
004dca2c  pop        ebx
004dca2d  add        esp, 0x30c
004dca33  ret        

// block 004dca34
004dca34  xor        esi, esi

// block 004dca36
004dca36  mov        ecx, edi
004dca38  call       0x4dc821

// block 004dca3d
004dca3d  cmp        eax, 0x10
004dca40  jae        0x4dca57

// block 004dca42
004dca42  mov        ecx, dword ptr [ebx + 0x260]
004dca48  mov        dl, byte ptr [ecx + esi]
004dca4b  add        dl, al
004dca4d  and        dl, 0xf
004dca50  mov        byte ptr [esp + esi + 0x24], dl
004dca54  inc        esi
004dca55  jmp        0x4dcab7

// block 004dca57
004dca57  jne        0x4dca81

// block 004dca59
004dca59  push       2
004dca5b  mov        ecx, ebp
004dca5d  call       0x4dc615

// block 004dca62
004dca62  add        eax, 3
004dca65  test       eax, eax
004dca67  jle        0x4dcab7

// block 004dca69
004dca69  cmp        esi, 0x2f5
004dca6f  jge        0x4dcac3

// block 004dca71
004dca71  mov        cl, byte ptr [esp + esi + 0x23]
004dca75  dec        eax
004dca76  mov        byte ptr [esp + esi + 0x24], cl
004dca7a  inc        esi
004dca7b  test       eax, eax
004dca7d  jg         0x4dca69

// block 004dca7f
004dca7f  jmp        0x4dcab7

// block 004dca81
004dca81  cmp        eax, 0x11
004dca84  jne        0x4dca94

// block 004dca86
004dca86  push       3
004dca88  mov        ecx, ebp
004dca8a  call       0x4dc615

// block 004dca8f
004dca8f  add        eax, 3
004dca92  jmp        0x4dcaa0

// block 004dca94
004dca94  push       7
004dca96  mov        ecx, ebp
004dca98  call       0x4dc615

// block 004dca9d
004dca9d  add        eax, 0xb

// block 004dcaa0
004dcaa0  test       eax, eax
004dcaa2  jle        0x4dcab7

// block 004dcaa4
004dcaa4  cmp        esi, 0x2f5
004dcaaa  jge        0x4dcac3

// block 004dcaac
004dcaac  mov        byte ptr [esp + esi + 0x24], 0
004dcab1  inc        esi
004dcab2  dec        eax
004dcab3  test       eax, eax
004dcab5  jg         0x4dcaa4

// block 004dcab7
004dcab7  cmp        esi, 0x2f5
004dcabd  jl         0x4dca36

// block 004dcac3
004dcac3  lea        edx, [esp + 0x24]
004dcac7  lea        ecx, [ebx + 0x10]
004dcaca  push       edx
004dcacb  call       0x4dc6a5

// block 004dcad0
004dcad0  test       al, al
004dcad2  jne        0x4dcadf

// block 004dcad4
004dcad4  pop        edi
004dcad5  pop        esi
004dcad6  pop        ebp
004dcad7  pop        ebx
004dcad8  add        esp, 0x30c
004dcade  ret        

// block 004dcadf
004dcadf  lea        eax, [esp + 0x2f5]
004dcae6  lea        ecx, [ebx + 0xa0]
004dcaec  push       eax
004dcaed  call       0x4dc6a5

// block 004dcaf2
004dcaf2  test       al, al
004dcaf4  jne        0x4dcb01

// block 004dcaf6
004dcaf6  pop        edi
004dcaf7  pop        esi
004dcaf8  pop        ebp
004dcaf9  pop        ebx
004dcafa  add        esp, 0x30c
004dcb00  ret        

// block 004dcb01
004dcb01  lea        ecx, [esp + 0x311]
004dcb08  push       ecx
004dcb09  lea        ecx, [ebx + 0x130]
004dcb0f  call       0x4dc6a5

// block 004dcb14
004dcb14  test       al, al
004dcb16  jne        0x4dcb23

// block 004dcb18
004dcb18  pop        edi
004dcb19  pop        esi
004dcb1a  pop        ebp
004dcb1b  pop        ebx
004dcb1c  add        esp, 0x30c
004dcb22  ret        

// block 004dcb23
004dcb23  mov        byte ptr [ebx + 0x264], 0
004dcb2a  xor        eax, eax

// block 004dcb2c
004dcb2c  cmp        byte ptr [esp + eax + 0x311], 3
004dcb34  jne        0x4dcb3e

// block 004dcb36
004dcb36  inc        eax
004dcb37  cmp        eax, 8
004dcb3a  jb         0x4dcb2c

// block 004dcb3c
004dcb3c  jmp        0x4dcb45

// block 004dcb3e
004dcb3e  mov        byte ptr [ebx + 0x264], 1

// block 004dcb45
004dcb45  mov        edi, dword ptr [ebx + 0x260]
004dcb4b  lea        esi, [esp + 0x24]
004dcb4f  mov        ecx, 0x2f5

// block 004dcb54
004dcb54  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 004dcb56
004dcb56  pop        edi
004dcb57  pop        esi
004dcb58  pop        ebp
004dcb59  mov        al, 1
004dcb5b  pop        ebx
004dcb5c  add        esp, 0x30c
004dcb62  ret        
