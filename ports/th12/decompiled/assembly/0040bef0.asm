
// block 0040bef0
0040bef0  fldz       
0040bef2  push       esi
0040bef3  sub        esp, 8
0040bef6  fst        dword ptr [esp + 4]
0040befa  mov        esi, eax
0040befc  fstp       dword ptr [esp]
0040beff  lea        ecx, [esi + 0x4bc]
0040bf05  call       0x40d560

// block 0040bf0a
0040bf0a  test       eax, eax
0040bf0c  je         0x40bfee

// block 0040bf12
0040bf12  push       edi
0040bf13  xor        edi, edi
0040bf15  test       byte ptr [esi + 0x800], 1
0040bf1c  je         0x40bf2e

// block 0040bf1e
0040bf1e  mov        ecx, esi
0040bf20  call       0x40be50

// block 0040bf25
0040bf25  test       eax, eax
0040bf27  je         0x40bf2e

// block 0040bf29
0040bf29  mov        edi, 1

// block 0040bf2e
0040bf2e  test       byte ptr [esi + 0x800], 2
0040bf35  je         0x40bf47

// block 0040bf37
0040bf37  mov        ecx, esi
0040bf39  call       0x40be90

// block 0040bf3e
0040bf3e  test       eax, eax
0040bf40  je         0x40bf47

// block 0040bf42
0040bf42  mov        edi, 1

// block 0040bf47
0040bf47  test       byte ptr [esi + 0x800], 8
0040bf4e  je         0x40bf5e

// block 0040bf50
0040bf50  call       0x40bde0

// block 0040bf55
0040bf55  test       eax, eax
0040bf57  je         0x40bf5e

// block 0040bf59
0040bf59  mov        edi, 1

// block 0040bf5e
0040bf5e  test       byte ptr [esi + 0x800], 4
0040bf65  je         0x40bf75

// block 0040bf67
0040bf67  call       0x40bd70

// block 0040bf6c
0040bf6c  test       eax, eax
0040bf6e  je         0x40bf75

// block 0040bf70
0040bf70  mov        edi, 1

// block 0040bf75
0040bf75  fld        dword ptr [esi + 0x7e4]
0040bf7b  fcomp      qword ptr [0x4a3e70]
0040bf81  fnstsw     ax
0040bf83  test       ah, 0x41
0040bf86  jne        0x40bf94

// block 0040bf88
0040bf88  fld        dword ptr [esi + 0x7e4]
0040bf8e  fstp       dword ptr [esi + 0x4d4]

// block 0040bf94
0040bf94  fld        dword ptr [esi + 0x4d4]
0040bf9a  sub        esp, 8
0040bf9d  fstp       dword ptr [esp + 4]
0040bfa1  lea        ecx, [esi + 0x4c8]
0040bfa7  fld        dword ptr [esi + 0x4d8]
0040bfad  fstp       dword ptr [esp]
0040bfb0  call       0x40d640

// block 0040bfb5
0040bfb5  test       edi, edi
0040bfb7  pop        edi
0040bfb8  je         0x40bfcf

// block 0040bfba
0040bfba  mov        edx, dword ptr [esi + 0x544]
0040bfc0  inc        dword ptr [esi + 0x7f8]
0040bfc6  test       edx, edx
0040bfc8  jl         0x40bfcf

// block 0040bfca
0040bfca  call       0x453d90

// block 0040bfcf
0040bfcf  mov        eax, dword ptr [esi + 0x7f8]
0040bfd5  cmp        eax, dword ptr [esi + 0x7fc]
0040bfdb  jl         0x40bfee

// block 0040bfdd
0040bfdd  and        dword ptr [esi + 0x528], 0xfffffeff
0040bfe7  mov        eax, 1
0040bfec  pop        esi
0040bfed  ret        

// block 0040bfee
0040bfee  xor        eax, eax
0040bff0  pop        esi
0040bff1  ret        
