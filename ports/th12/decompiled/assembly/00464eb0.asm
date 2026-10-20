
// block 00464eb0
00464eb0  mov        eax, dword ptr [ebx + 0x30]
00464eb3  sub        esp, 0x1c
00464eb6  push       esi
00464eb7  push       edi
00464eb8  test       al, 1
00464eba  je         0x464f6d

// block 00464ec0
00464ec0  fld        dword ptr [ebx + 0x20]
00464ec3  test       al, 2
00464ec5  jne        0x464edf

// block 00464ec7
00464ec7  sub        esp, 8
00464eca  fstp       dword ptr [esp + 4]
00464ece  lea        ecx, [esp + 0x20]
00464ed2  fld        dword ptr [ebx + 0x1c]
00464ed5  fstp       dword ptr [esp]
00464ed8  call       0x465390

// block 00464edd
00464edd  jmp        0x464f36

// block 00464edf
00464edf  sub        esp, 0xc
00464ee2  fstp       dword ptr [esp + 8]
00464ee6  fld        dword ptr [ebx + 0x28]
00464ee9  fstp       dword ptr [esp + 4]
00464eed  fld        dword ptr [ebx + 0x1c]
00464ef0  fstp       dword ptr [esp]
00464ef3  call       0x465280

// block 00464ef8
00464ef8  push       ecx
00464ef9  fstp       dword ptr [esp]
00464efc  call       0x4646e0

// block 00464f01
00464f01  push       ecx
00464f02  fstp       dword ptr [esp]
00464f05  call       0x4646e0

// block 00464f0a
00464f0a  push       ecx
00464f0b  lea        ecx, [esp + 0x14]
00464f0f  fstp       dword ptr [esp]
00464f12  call       0x465390

// block 00464f17
00464f17  fld        dword ptr [ebx + 0x2c]
00464f1a  fmul       dword ptr [esp + 0xc]
00464f1e  push       ecx
00464f1f  lea        esi, [esp + 0x10]
00464f23  lea        edi, [esp + 0x1c]
00464f27  fstp       dword ptr [esp + 0x10]
00464f2b  fld        dword ptr [ebx + 0x28]
00464f2e  fstp       dword ptr [esp]
00464f31  call       0x464780

// block 00464f36
00464f36  fld        dword ptr [ebx + 0xc]
00464f39  fadd       dword ptr [esp + 0x18]
00464f3d  fstp       dword ptr [esp + 0xc]
00464f41  mov        eax, dword ptr [esp + 0xc]
00464f45  fld        dword ptr [ebx + 0x10]
00464f48  fadd       dword ptr [esp + 0x1c]
00464f4c  fstp       dword ptr [esp + 0x10]
00464f50  mov        ecx, dword ptr [esp + 0x10]
00464f54  fld        dword ptr [ebx + 0x14]
00464f57  mov        dword ptr [ebx], eax
00464f59  fadd       qword ptr [0x4a3d58]
00464f5f  mov        dword ptr [ebx + 4], ecx
00464f62  fstp       dword ptr [esp + 0x14]
00464f66  mov        edx, dword ptr [esp + 0x14]
00464f6a  mov        dword ptr [ebx + 8], edx

// block 00464f6d
00464f6d  mov        esi, ebx
00464f6f  call       0x465320

// block 00464f74
00464f74  pop        edi
00464f75  pop        esi
00464f76  add        esp, 0x1c
00464f79  ret        
