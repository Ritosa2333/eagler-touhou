
// block 0045d710
0045d710  push       ecx
0045d711  mov        eax, dword ptr [esp + 0x1c]
0045d715  push       ebx
0045d716  push       ebp
0045d717  mov        ebp, dword ptr [esp + 0x10]
0045d71b  push       esi
0045d71c  mov        esi, dword ptr [ebp + 0x8856b0]
0045d722  lea        ebx, [eax + eax*4]
0045d725  push       edi
0045d726  lea        edi, [ebp + 0x8856b0]
0045d72c  add        ebx, ebx
0045d72e  add        ebx, ebx
0045d730  lea        ecx, [esi + ebx + 0x14]
0045d734  cmp        ecx, edi
0045d736  jae        0x45d87d

// block 0045d73c
0045d73c  fld        dword ptr [esp + 0x28]
0045d740  inc        eax
0045d741  fstp       dword ptr [esp + 0x18]
0045d745  fild       dword ptr [esp + 0x2c]
0045d749  fdivr      qword ptr [0x4a3cd0]
0045d74f  fstp       dword ptr [esp + 0x28]
0045d753  test       eax, eax
0045d755  jle        0x45d7c2

// block 0045d757
0045d757  mov        dword ptr [esp + 0x10], eax
0045d75b  jmp        0x45d760

// block 0045d760
0045d760  fld        dword ptr [esp + 0x24]
0045d764  sub        esp, 8
0045d767  fstp       dword ptr [esp + 4]
0045d76b  mov        ecx, esi
0045d76d  fld        dword ptr [esp + 0x20]
0045d771  fstp       dword ptr [esp]
0045d774  call       0x45d890

// block 0045d779
0045d779  fld        dword ptr [esp + 0x1c]
0045d77d  mov        edx, dword ptr [esp + 0x30]
0045d781  fadd       dword ptr [esi]
0045d783  sub        esp, 8
0045d786  mov        dword ptr [esi + 0x10], edx
0045d789  add        esi, 0x14
0045d78c  fstp       dword ptr [esi - 0x14]
0045d78f  fld        dword ptr [esi - 0x10]
0045d792  fadd       dword ptr [esp + 0x28]
0045d796  fstp       dword ptr [esi - 0x10]
0045d799  fldz       
0045d79b  fstp       dword ptr [esi - 0xc]
0045d79e  fld1       
0045d7a0  fstp       dword ptr [esi - 8]
0045d7a3  fld        dword ptr [esp + 0x30]
0045d7a7  fstp       dword ptr [esp + 4]
0045d7ab  fld        dword ptr [esp + 0x20]
0045d7af  fstp       dword ptr [esp]
0045d7b2  call       0x464640

// block 0045d7b7
0045d7b7  fstp       dword ptr [esp + 0x18]
0045d7bb  sub        dword ptr [esp + 0x10], 1
0045d7c0  jne        0x45d760

// block 0045d7c2
0045d7c2  mov        eax, dword ptr [0x4ce8cc]
0045d7c7  cmp        byte ptr [eax + 0x4b5647], 0
0045d7ce  je         0x45d808

// block 0045d7d0
0045d7d0  mov        eax, dword ptr [0x4ce8f0]
0045d7d5  mov        ecx, dword ptr [eax]
0045d7d7  mov        edx, dword ptr [ecx + 0x10c]
0045d7dd  push       3
0045d7df  push       4
0045d7e1  push       0
0045d7e3  push       eax
0045d7e4  call       edx

// block 0045d7e6
0045d7e6  mov        eax, dword ptr [0x4ce8f0]
0045d7eb  mov        ecx, dword ptr [eax]
0045d7ed  mov        edx, dword ptr [ecx + 0x10c]
0045d7f3  push       3
0045d7f5  push       1
0045d7f7  push       0
0045d7f9  push       eax
0045d7fa  call       edx

// block 0045d7fc
0045d7fc  mov        eax, dword ptr [0x4ce8cc]
0045d801  mov        byte ptr [eax + 0x4b5647], 0

// block 0045d808
0045d808  cmp        byte ptr [ebp + 0x4b5642], 1
0045d80f  je         0x45d844

// block 0045d811
0045d811  mov        eax, dword ptr [0x4ce8f0]
0045d816  mov        ecx, dword ptr [eax]
0045d818  mov        edx, dword ptr [ecx + 0x10c]
0045d81e  push       0
0045d820  push       6
0045d822  push       0
0045d824  push       eax
0045d825  call       edx

// block 0045d827
0045d827  mov        eax, dword ptr [0x4ce8f0]
0045d82c  mov        ecx, dword ptr [eax]
0045d82e  mov        edx, dword ptr [ecx + 0x10c]
0045d834  push       0
0045d836  push       3
0045d838  push       0
0045d83a  push       eax
0045d83b  call       edx

// block 0045d83d
0045d83d  mov        byte ptr [ebp + 0x4b5642], 1

// block 0045d844
0045d844  mov        eax, dword ptr [0x4ce8f0]
0045d849  mov        ecx, dword ptr [eax]
0045d84b  mov        edx, dword ptr [ecx + 0x164]
0045d851  push       0x44
0045d853  push       eax
0045d854  call       edx

// block 0045d856
0045d856  mov        edx, dword ptr [edi]
0045d858  mov        eax, dword ptr [0x4ce8f0]
0045d85d  mov        ecx, dword ptr [eax]
0045d85f  push       0x14
0045d861  push       edx
0045d862  mov        edx, dword ptr [esp + 0x34]
0045d866  push       edx
0045d867  push       3
0045d869  push       eax
0045d86a  mov        eax, dword ptr [ecx + 0x14c]
0045d870  call       eax

// block 0045d872
0045d872  add        ebx, 0x14
0045d875  add        dword ptr [edi], ebx
0045d877  inc        dword ptr [ebp + 0xac]

// block 0045d87d
0045d87d  pop        edi
0045d87e  pop        esi
0045d87f  pop        ebp
0045d880  xor        eax, eax
0045d882  pop        ebx
0045d883  pop        ecx
0045d884  ret        0x1c
