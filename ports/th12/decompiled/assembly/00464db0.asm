
// block 00464db0
00464db0  mov        eax, dword ptr [ebx + 0x30]
00464db3  sub        esp, 0x1c
00464db6  push       esi
00464db7  push       edi
00464db8  test       al, 1
00464dba  jne        0x464de2

// block 00464dbc
00464dbc  fld        dword ptr [ebx + 0xc]
00464dbf  mov        esi, ebx
00464dc1  fadd       dword ptr [ebx]
00464dc3  fstp       dword ptr [ebx]
00464dc5  fld        dword ptr [ebx + 0x10]
00464dc8  fadd       dword ptr [ebx + 4]
00464dcb  fstp       dword ptr [ebx + 4]
00464dce  fld        dword ptr [ebx + 0x14]
00464dd1  fadd       dword ptr [ebx + 8]
00464dd4  fstp       dword ptr [ebx + 8]
00464dd7  call       0x465320

// block 00464ddc
00464ddc  pop        edi
00464ddd  pop        esi
00464dde  add        esp, 0x1c
00464de1  ret        

// block 00464de2
00464de2  fld        dword ptr [ebx + 0x20]
00464de5  test       al, 2
00464de7  jne        0x464e08

// block 00464de9
00464de9  sub        esp, 8
00464dec  fstp       dword ptr [esp + 4]
00464df0  lea        ecx, [esp + 0x20]
00464df4  fld        dword ptr [ebx + 0x1c]
00464df7  fstp       dword ptr [esp]
00464dfa  call       0x465390

// block 00464dff
00464dff  fld        dword ptr [esp + 0x18]
00464e03  fadd       dword ptr [ebx + 0xc]
00464e06  jmp        0x464e66

// block 00464e08
00464e08  sub        esp, 0xc
00464e0b  fstp       dword ptr [esp + 8]
00464e0f  fld        dword ptr [ebx + 0x28]
00464e12  fstp       dword ptr [esp + 4]
00464e16  fld        dword ptr [ebx + 0x1c]
00464e19  fstp       dword ptr [esp]
00464e1c  call       0x465280

// block 00464e21
00464e21  push       ecx
00464e22  fstp       dword ptr [esp]
00464e25  call       0x4646e0

// block 00464e2a
00464e2a  push       ecx
00464e2b  fstp       dword ptr [esp]
00464e2e  call       0x4646e0

// block 00464e33
00464e33  push       ecx
00464e34  lea        ecx, [esp + 0x14]
00464e38  fstp       dword ptr [esp]
00464e3b  call       0x465390

// block 00464e40
00464e40  fld        dword ptr [ebx + 0x2c]
00464e43  fmul       dword ptr [esp + 0xc]
00464e47  push       ecx
00464e48  lea        esi, [esp + 0x10]
00464e4c  lea        edi, [esp + 0x1c]
00464e50  fstp       dword ptr [esp + 0x10]
00464e54  fld        dword ptr [ebx + 0x28]
00464e57  fstp       dword ptr [esp]
00464e5a  call       0x464780

// block 00464e5f
00464e5f  fld        dword ptr [ebx + 0xc]
00464e62  fadd       dword ptr [esp + 0x18]

// block 00464e66
00464e66  fstp       dword ptr [esp + 0xc]
00464e6a  mov        eax, dword ptr [esp + 0xc]
00464e6e  fld        dword ptr [ebx + 0x10]
00464e71  mov        esi, ebx
00464e73  fadd       dword ptr [esp + 0x1c]
00464e77  fstp       dword ptr [esp + 0x10]
00464e7b  mov        ecx, dword ptr [esp + 0x10]
00464e7f  fld        dword ptr [ebx + 0x14]
00464e82  mov        dword ptr [ebx], eax
00464e84  fadd       qword ptr [0x4a3d58]
00464e8a  mov        dword ptr [ebx + 4], ecx
00464e8d  fstp       dword ptr [esp + 0x14]
00464e91  mov        edx, dword ptr [esp + 0x14]
00464e95  mov        dword ptr [ebx + 8], edx
00464e98  call       0x465320

// block 00464e9d
00464e9d  pop        edi
00464e9e  pop        esi
00464e9f  add        esp, 0x1c
00464ea2  ret        
