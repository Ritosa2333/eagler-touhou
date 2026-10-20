
// block 00496afd
00496afd  mov        edi, edi
00496aff  push       ebp
00496b00  mov        ebp, esp
00496b02  fldz       
00496b04  fcom       qword ptr [ebp + 8]
00496b07  fnstsw     ax
00496b09  test       ah, 0x44
00496b0c  jp         0x496b15

// block 00496b0e
00496b0e  xor        edx, edx
00496b10  jmp        0x496baf

// block 00496b15
00496b15  mov        edx, dword ptr [ebp + 0xe]
00496b18  xor        ecx, ecx
00496b1a  test       edx, 0x7ff0
00496b20  jne        0x496b8d

// block 00496b22
00496b22  test       dword ptr [ebp + 0xc], 0xfffff
00496b29  jne        0x496b30

// block 00496b2b
00496b2b  cmp        dword ptr [ebp + 8], ecx
00496b2e  je         0x496b8d

// block 00496b30
00496b30  fcomp      qword ptr [ebp + 8]
00496b33  mov        edx, 0xfffffc03
00496b38  fnstsw     ax
00496b3a  test       ah, 0x41
00496b3d  jne        0x496b44

// block 00496b3f
00496b3f  xor        eax, eax
00496b41  inc        eax
00496b42  jmp        0x496b5c

// block 00496b44
00496b44  xor        eax, eax
00496b46  jmp        0x496b5c

// block 00496b48
00496b48  shl        dword ptr [ebp + 0xc], 1
00496b4b  test       dword ptr [ebp + 8], 0x80000000
00496b52  je         0x496b58

// block 00496b54
00496b54  or         dword ptr [ebp + 0xc], 1

// block 00496b58
00496b58  shl        dword ptr [ebp + 8], 1
00496b5b  dec        edx

// block 00496b5c
00496b5c  test       byte ptr [ebp + 0xe], 0x10
00496b60  je         0x496b48

// block 00496b62
00496b62  push       esi
00496b63  mov        esi, 0xffef
00496b68  and        word ptr [ebp + 0xe], si
00496b6c  pop        esi
00496b6d  cmp        eax, ecx
00496b6f  je         0x496b7a

// block 00496b71
00496b71  mov        eax, 0x8000
00496b76  or         word ptr [ebp + 0xe], ax

// block 00496b7a
00496b7a  fld        qword ptr [ebp + 8]
00496b7d  push       ecx
00496b7e  push       ecx
00496b7f  push       ecx
00496b80  fstp       qword ptr [esp]
00496b83  call       0x496a05

// block 00496b88
00496b88  add        esp, 0xc
00496b8b  jmp        0x496baf

// block 00496b8d
00496b8d  push       ecx
00496b8e  fstp       st(0)
00496b90  fld        qword ptr [ebp + 8]
00496b93  push       ecx
00496b94  push       ecx
00496b95  fstp       qword ptr [esp]
00496b98  call       0x496a05

// block 00496b9d
00496b9d  shr        edx, 4
00496ba0  and        edx, 0x7ff
00496ba6  add        esp, 0xc
00496ba9  sub        edx, 0x3fe

// block 00496baf
00496baf  mov        eax, dword ptr [ebp + 0x10]
00496bb2  mov        dword ptr [eax], edx
00496bb4  pop        ebp
00496bb5  ret        
