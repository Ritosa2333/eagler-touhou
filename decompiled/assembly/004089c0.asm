
// block 004089c0
004089c0  fld        dword ptr [0x4a3d78]
004089c6  sub        esp, 0x10
004089c9  push       ebx
004089ca  push       ebp
004089cb  mov        ebp, dword ptr [esp + 0x1c]
004089cf  fstp       dword ptr [ebp + 0x514]
004089d5  push       esi
004089d6  mov        esi, dword ptr [0x4b4514]
004089dc  fld        dword ptr [0x4a4114]
004089e2  fstp       dword ptr [ebp + 0x518]
004089e8  mov        eax, dword ptr [esi + 0x97c]
004089ee  push       edi
004089ef  lea        edi, [ebp + 0x508]
004089f5  mov        dword ptr [edi], eax
004089f7  mov        ecx, dword ptr [esi + 0x980]
004089fd  mov        dword ptr [edi + 4], ecx
00408a00  mov        edx, dword ptr [esi + 0x984]
00408a06  mov        dword ptr [edi + 8], edx
00408a09  mov        edx, 0x33
00408a0e  call       0x453d90

// block 00408a13
00408a13  mov        ecx, dword ptr [esi + 0x10]
00408a16  push       0
00408a18  push       0x13
00408a1a  lea        eax, [esp + 0x2c]
00408a1e  push       eax
00408a1f  push       ecx
00408a20  mov        eax, 0x17
00408a25  xor        ecx, ecx
00408a27  call       0x4615a0

// block 00408a2c
00408a2c  mov        edx, dword ptr [esp + 0x24]
00408a30  mov        ecx, dword ptr [0x4b43cc]
00408a36  mov        dword ptr [ebp + 0x40], edx
00408a39  mov        eax, dword ptr [ecx + 0x7c]
00408a3c  test       al, 1
00408a3e  je         0x408a67

// block 00408a40
00408a40  cmp        dword ptr [ecx + 0x28], 0x3c
00408a44  jl         0x408a55

// block 00408a46
00408a46  mov        dword ptr [ecx + 0x80], 0
00408a50  and        eax, 0xffffffdd
00408a53  jmp        0x408a64

// block 00408a55
00408a55  mov        edx, dword ptr [0x4b43c4]
00408a5b  cmp        dword ptr [edx + 0x3c], 0
00408a5f  je         0x408a67

// block 00408a61
00408a61  or         eax, 0x20

// block 00408a64
00408a64  mov        dword ptr [ecx + 0x7c], eax

// block 00408a67
00408a67  mov        eax, dword ptr [0x4b43dc]
00408a6c  inc        dword ptr [eax + 0x14]
00408a6f  push       0x44
00408a71  call       0x46c9ea

// block 00408a76
00408a76  mov        esi, eax
00408a78  add        esp, 4
00408a7b  test       esi, esi
00408a7d  je         0x408aa8

// block 00408a7f
00408a7f  and        dword ptr [esi + 0x40], 0xfffffffe
00408a83  push       0x44
00408a85  push       0
00408a87  push       esi
00408a88  call       0x477420

// block 00408a8d
00408a8d  or         dword ptr [esi], 2
00408a90  add        esp, 0xc
00408a93  push       0x46
00408a95  push       1
00408a97  push       0x3c
00408a99  push       0xb4
00408a9e  push       2
00408aa0  push       8
00408aa2  push       esi
00408aa3  call       0x452a60

// block 00408aa8
00408aa8  test       dword ptr [0x4cee78], 0x8000
00408ab2  mov        eax, dword ptr [0x4b43e4]
00408ab7  mov        ebx, dword ptr [eax + 0x6d44]
00408abd  je         0x408ad0

// block 00408abf
00408abf  push       0x4cf1d0
00408ac4  call       dword ptr [0x498088]

// block 00408aca
00408aca  inc        byte ptr [0x4cf221]

// block 00408ad0
00408ad0  inc        dword ptr [ebx + 0x130]
00408ad6  call       0x4621c0

// block 00408adb
00408adb  mov        esi, eax
00408add  or         dword ptr [esi + 0x480], 1
00408ae4  mov        dword ptr [esi + 0x20], 0x17
00408aeb  test       edi, edi
00408aed  jne        0x408b1d

// block 00408aef
00408aef  fldz       
00408af1  fst        dword ptr [esp + 0x14]
00408af5  mov        ecx, dword ptr [esp + 0x14]
00408af9  fst        dword ptr [esp + 0x18]
00408afd  mov        edx, dword ptr [esp + 0x18]
00408b01  fstp       dword ptr [esp + 0x1c]
00408b05  mov        eax, dword ptr [esp + 0x1c]
00408b09  mov        dword ptr [esi + 0x430], ecx
00408b0f  mov        dword ptr [esi + 0x434], edx
00408b15  mov        dword ptr [esi + 0x438], eax
00408b1b  jmp        0x408b49

// block 00408b1d
00408b1d  fld        dword ptr [edi]
00408b1f  fadd       qword ptr [0x4a3d70]
00408b25  fadd       qword ptr [0x4a3d28]
00408b2b  fstp       dword ptr [esi + 0x430]
00408b31  fld        dword ptr [edi + 4]
00408b34  fadd       qword ptr [0x4a3d68]
00408b3a  fstp       dword ptr [esi + 0x434]
00408b40  fld        dword ptr [edi + 8]
00408b43  fstp       dword ptr [esi + 0x438]

// block 00408b49
00408b49  push       1
00408b4b  push       esi
00408b4c  mov        ecx, ebx
00408b4e  call       0x454d10

// block 00408b53
00408b53  mov        ebx, esi
00408b55  lea        eax, [esp + 0x24]
00408b59  call       0x461250

// block 00408b5e
00408b5e  test       dword ptr [0x4cee78], 0x8000
00408b68  mov        esi, dword ptr [eax]
00408b6a  je         0x408b7d

// block 00408b6c
00408b6c  push       0x4cf1d0
00408b71  call       dword ptr [0x49808c]

// block 00408b77
00408b77  dec        byte ptr [0x4cf221]

// block 00408b7d
00408b7d  pop        edi
00408b7e  mov        dword ptr [ebp + 0x48], esi
00408b81  pop        esi
00408b82  pop        ebp
00408b83  xor        eax, eax
00408b85  pop        ebx
00408b86  add        esp, 0x10
00408b89  ret        4
