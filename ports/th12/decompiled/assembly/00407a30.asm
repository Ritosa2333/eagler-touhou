
// block 00407a30
00407a30  sub        esp, 0x18
00407a33  fldz       
00407a35  mov        eax, dword ptr [ecx + 0x18]
00407a38  cmp        eax, 0x1e
00407a3b  fst        dword ptr [esp]
00407a3e  fst        dword ptr [esp + 8]
00407a42  jge        0x407a4c

// block 00407a44
00407a44  fstp       st(0)
00407a46  xor        eax, eax
00407a48  add        esp, 0x18
00407a4b  ret        

// block 00407a4c
00407a4c  cmp        eax, 0x5a
00407a4f  fld        qword ptr [0x4a3d50]
00407a55  jge        0x407a6e

// block 00407a57
00407a57  fstp       st(1)
00407a59  fld        dword ptr [ecx + 0x1c]
00407a5c  fsub       qword ptr [0x4a3e78]
00407a62  fmul       st(1)
00407a64  fdiv       qword ptr [0x4a3d20]
00407a6a  fsubr      st(1)
00407a6c  jmp        0x407a70

// block 00407a6e
00407a6e  fxch       st(1)

// block 00407a70
00407a70  mov        eax, dword ptr [0x4b43cc]
00407a75  fstp       dword ptr [esp + 4]
00407a79  fld        dword ptr [0x4a40f0]
00407a7f  mov        ecx, dword ptr [eax + 0x7c]
00407a82  fstp       dword ptr [esp + 0xc]
00407a86  push       esi
00407a87  fld        dword ptr [esp + 8]
00407a8b  not        ecx
00407a8d  fld        st(0)
00407a8f  push       edi
00407a90  fsubr      st(2)
00407a92  and        ecx, 1
00407a95  push       ecx
00407a96  lea        edx, [esp + 0x18]
00407a9a  fstp       dword ptr [esp + 0x1c]
00407a9e  push       edx
00407a9f  lea        eax, [esp + 0x10]
00407aa3  faddp      st(1)
00407aa5  fmul       qword ptr [0x4a3cf8]
00407aab  fstp       dword ptr [esp + 0x14]
00407aaf  call       0x40cd00

// block 00407ab4
00407ab4  mov        eax, dword ptr [0x4b43cc]
00407ab9  mov        ecx, dword ptr [eax + 0x7c]
00407abc  not        ecx
00407abe  and        ecx, 1
00407ac1  push       ecx
00407ac2  lea        esi, [esp + 0x18]
00407ac6  lea        edi, [esp + 0xc]
00407aca  call       0x4285f0

// block 00407acf
00407acf  pop        edi
00407ad0  pop        esi
00407ad1  xor        eax, eax
00407ad3  add        esp, 0x18
00407ad6  ret        
