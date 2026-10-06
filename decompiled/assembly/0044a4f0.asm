
// block 0044a4f0
0044a4f0  push       ebp
0044a4f1  mov        ebp, esp
0044a4f3  and        esp, 0xfffffff8
0044a4f6  sub        esp, 0x10
0044a4f9  cmp        dword ptr [ebx + 0x38], 0
0044a4fd  push       esi
0044a4fe  push       edi
0044a4ff  je         0x44a5b7

// block 0044a505
0044a505  cmp        dword ptr [ebx + 0x14], 0x14
0044a509  jl         0x44a5b7

// block 0044a50f
0044a50f  fld        dword ptr [0x4a4100]
0044a515  mov        esi, dword ptr [0x4b43b8]
0044a51b  fstp       dword ptr [esp + 0xc]
0044a51f  mov        dword ptr [esi + 0x18f98], 3
0044a529  fld        dword ptr [0x4a4178]
0044a52f  mov        eax, dword ptr [ebx + 0x48]
0044a532  fstp       dword ptr [esp + 0x10]
0044a536  push       eax
0044a537  fldz       
0044a539  push       0x4a1298
0044a53e  lea        edi, [esp + 0x14]
0044a542  fstp       dword ptr [esp + 0x1c]
0044a546  call       0x4020a0

// block 0044a54b
0044a54b  fld        dword ptr [esp + 0x18]
0044a54f  mov        ecx, dword ptr [ebx + 0x4c]
0044a552  fadd       qword ptr [0x4a4170]
0044a558  mov        esi, dword ptr [0x4b43b8]
0044a55e  push       ecx
0044a55f  push       0x4a1298
0044a564  fstp       dword ptr [esp + 0x20]
0044a568  call       0x4020a0

// block 0044a56d
0044a56d  fld        dword ptr [esp + 0x20]
0044a571  mov        esi, dword ptr [0x4b43b8]
0044a577  fadd       qword ptr [0x4a3e78]
0044a57d  mov        dword ptr [esi + 0x18f98], 2
0044a587  fstp       dword ptr [esp + 0x20]
0044a58b  fld        dword ptr [ebx + 0x50]
0044a58e  fmul       qword ptr [0x4a3ce8]
0044a594  call       0x4931e0

// block 0044a599
0044a599  push       eax
0044a59a  push       0x4a214c
0044a59f  call       0x4020a0

// block 0044a5a4
0044a5a4  mov        edx, dword ptr [0x4b43b8]
0044a5aa  add        esp, 0x18
0044a5ad  mov        dword ptr [edx + 0x18f98], 0

// block 0044a5b7
0044a5b7  pop        edi
0044a5b8  mov        eax, 1
0044a5bd  pop        esi
0044a5be  mov        esp, ebp
0044a5c0  pop        ebp
0044a5c1  ret        
