
// block 0041bb40
0041bb40  push       ebp
0041bb41  mov        ebp, esp
0041bb43  and        esp, 0xfffffff8
0041bb46  sub        esp, 0x1f0
0041bb4c  push       ebx
0041bb4d  push       ebp
0041bb4e  push       esi
0041bb4f  mov        esi, dword ptr [0x4b43c8]
0041bb55  push       edi
0041bb56  push       0x1e8
0041bb5b  lea        eax, [esp + 0x1c]
0041bb5f  push       0
0041bb61  push       eax
0041bb62  add        esi, 0x64
0041bb65  call       0x477420

// block 0041bb6a
0041bb6a  push       0x1b0
0041bb6f  lea        ecx, [esp + 0x58]
0041bb73  push       0
0041bb75  push       ecx
0041bb76  call       0x477420

// block 0041bb7b
0041bb7b  fld        dword ptr [0x4a3db8]
0041bb81  mov        ebx, dword ptr [0x4b44f4]
0041bb87  add        esp, 0x18
0041bb8a  mov        dword ptr [esp + 0x14], 0x7d0

// block 0041bb92
0041bb92  mov        ecx, 1
0041bb97  test       byte ptr [esi], cl
0041bb99  je         0x41bd00

// block 0041bb9f
0041bb9f  cmp        word ptr [esi + 0x532], cx
0041bba6  jne        0x41bd00

// block 0041bbac
0041bbac  cmp        dword ptr [esi + 0x630], ecx
0041bbb2  jne        0x41bd00

// block 0041bbb8
0041bbb8  fld        dword ptr [esi + 0x628]
0041bbbe  mov        edx, dword ptr [esi + 0x4bc]
0041bbc4  mov        eax, dword ptr [esi + 0x4c0]
0041bbca  fstp       dword ptr [esp + 0x24]
0041bbce  fld        dword ptr [esi + 0x62c]
0041bbd4  or         dword ptr [esp + 0x44], ecx
0041bbd8  fstp       dword ptr [esp + 0x38]
0041bbdc  mov        dword ptr [esp + 0x18], edx
0041bbe0  mov        edx, dword ptr [esi + 0x4c4]
0041bbe6  fst        dword ptr [esp + 0x2c]
0041bbea  mov        dword ptr [esp + 0x1c], eax
0041bbee  fst        dword ptr [esp + 0x28]
0041bbf2  mov        dword ptr [esp + 0x20], edx
0041bbf6  fldz       
0041bbf8  mov        eax, 7
0041bbfd  fstp       dword ptr [esp + 0x30]
0041bc01  mov        edx, 6
0041bc06  fld        dword ptr [0x4a4110]
0041bc0c  lea        edi, [ebx + 0x468]
0041bc12  fstp       dword ptr [esp + 0x34]
0041bc16  mov        word ptr [esp + 0x3c], ax
0041bc1b  mov        word ptr [esp + 0x3e], dx
0041bc20  mov        dword ptr [esp + 0x1f8], 0x13
0041bc2b  mov        dword ptr [esp + 0x1fc], 0xffffffff
0041bc36  mov        dword ptr [esp + 0x58], 0x200
0041bc3e  mov        dword ptr [esp + 0x50], 0x4b0
0041bc46  cmp        dword ptr [edi], 0x100
0041bc4c  jl         0x41bc52

// block 0041bc4e
0041bc4e  xor        ebp, ebp
0041bc50  jmp        0x41bccc

// block 0041bc52
0041bc52  add        dword ptr [ebx + 0x46c], ecx
0041bc58  fstp       st(0)
0041bc5a  cmp        dword ptr [ebx + 0x46c], 0x10000
0041bc64  lea        ebp, [ebx + 0x46c]
0041bc6a  jge        0x41bc73

// block 0041bc6c
0041bc6c  mov        dword ptr [ebp], 0x10000

// block 0041bc73
0041bc73  push       0xfa4
0041bc78  call       0x46c9ea

// block 0041bc7d
0041bc7d  add        esp, 4
0041bc80  test       eax, eax
0041bc82  je         0x41bc8b

// block 0041bc84
0041bc84  call       0x428520

// block 0041bc89
0041bc89  jmp        0x41bc8d

// block 0041bc8b
0041bc8b  xor        eax, eax

// block 0041bc8d
0041bc8d  mov        ecx, dword ptr [ebp]
0041bc90  mov        dword ptr [eax + 0x80], ecx
0041bc96  mov        ecx, dword ptr [ebx + 0x464]
0041bc9c  mov        dword ptr [eax + 4], ecx
0041bc9f  mov        dword ptr [ecx + 8], eax
0041bca2  inc        dword ptr [edi]
0041bca4  mov        dword ptr [ebx + 0x464], eax
0041bcaa  mov        edx, dword ptr [eax]
0041bcac  mov        edx, dword ptr [edx + 4]
0041bcaf  lea        ecx, [esp + 0x18]
0041bcb3  push       ecx
0041bcb4  mov        ecx, eax
0041bcb6  call       edx

// block 0041bcb8
0041bcb8  fld        dword ptr [0x4a3db8]
0041bcbe  mov        ebp, dword ptr [ebp]
0041bcc1  mov        ebx, dword ptr [0x4b44f4]
0041bcc7  mov        ecx, 1

// block 0041bccc
0041bccc  mov        eax, dword ptr [ebx + 0x18]
0041bccf  test       ebp, ebp
0041bcd1  je         0x41bce6

// block 0041bcd3
0041bcd3  test       eax, eax
0041bcd5  je         0x41bce6

// block 0041bcd7
0041bcd7  cmp        dword ptr [eax + 0x80], ebp
0041bcdd  je         0x41bce8

// block 0041bcdf
0041bcdf  mov        eax, dword ptr [eax + 8]
0041bce2  test       eax, eax
0041bce4  jne        0x41bcd7

// block 0041bce6
0041bce6  xor        eax, eax

// block 0041bce8
0041bce8  fld        dword ptr [esi + 0x640]
0041bcee  fstp       dword ptr [eax + 0x454]
0041bcf4  fld        dword ptr [esi + 0x644]
0041bcfa  fstp       dword ptr [eax + 0x458]

// block 0041bd00
0041bd00  add        esi, 0x9f8
0041bd06  sub        dword ptr [esp + 0x14], ecx
0041bd0a  jne        0x41bb92

// block 0041bd10
0041bd10  pop        edi
0041bd11  fstp       st(0)
0041bd13  xor        eax, eax
0041bd15  pop        esi
0041bd16  pop        ebp
0041bd17  pop        ebx
0041bd18  mov        esp, ebp
0041bd1a  pop        ebp
0041bd1b  ret        
