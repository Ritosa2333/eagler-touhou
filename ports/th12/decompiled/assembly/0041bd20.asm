
// block 0041bd20
0041bd20  sub        esp, 0x24
0041bd23  push       ebx
0041bd24  push       ebp
0041bd25  push       edi
0041bd26  mov        edi, ecx
0041bd28  push       0x214
0041bd2d  lea        ebp, [edi + 0x1318]
0041bd33  push       0
0041bd35  push       ebp
0041bd36  call       0x477420

// block 0041bd3b
0041bd3b  fldz       
0041bd3d  mov        eax, 1
0041bd42  fstp       dword ptr [edi + 0x1328]
0041bd48  mov        ecx, 7
0041bd4d  mov        word ptr [ebp], cx
0041bd51  mov        word ptr [edi + 0x1514], ax
0041bd58  mov        word ptr [edi + 0x1510], ax
0041bd5f  xor        edx, edx
0041bd61  mov        ecx, eax
0041bd63  mov        eax, dword ptr [0x4b44f4]
0041bd68  mov        word ptr [edi + 0x131a], dx
0041bd6f  mov        word ptr [edi + 0x1512], cx
0041bd76  mov        dword ptr [edi + 0x151c], 0x16
0041bd80  mov        dword ptr [edi + 0x1520], 0x28
0041bd8a  mov        dword ptr [edi + 0x1518], 0x83
0041bd94  mov        ebx, dword ptr [eax + 0x18]
0041bd97  add        esp, 0xc
0041bd9a  mov        dword ptr [eax + 0x48c], ebx
0041bda0  test       ebx, ebx
0041bda2  je         0x41bf3d

// block 0041bda8
0041bda8  push       esi
0041bda9  lea        esp, [esp]

// block 0041bdb0
0041bdb0  mov        edx, dword ptr [ebx + 0x40]
0041bdb3  cmp        edx, dword ptr [edi + 0x23c]
0041bdb9  jl         0x41bf18

// block 0041bdbf
0041bdbf  fldz       
0041bdc1  mov        ecx, dword ptr [ebx + 0x54]
0041bdc4  mov        eax, dword ptr [ebx + 0x50]
0041bdc7  fstp       dword ptr [esp + 0x10]
0041bdcb  fld        dword ptr [edi + 0x24c]
0041bdd1  mov        edx, dword ptr [ebx + 0x58]
0041bdd4  sub        esp, 8
0041bdd7  fstp       dword ptr [esp + 4]
0041bddb  fld        dword ptr [ebx + 0x68]
0041bdde  mov        dword ptr [esp + 0x28], ecx
0041bde2  lea        ecx, [esp + 0x30]
0041bde6  fstp       dword ptr [esp]
0041bde9  mov        dword ptr [esp + 0x24], eax
0041bded  mov        dword ptr [esp + 0x2c], edx
0041bdf1  call       0x41c580

// block 0041bdf6
0041bdf6  fld        dword ptr [ebx + 0x6c]
0041bdf9  fcomp      qword ptr [0x4a3d58]
0041bdff  fnstsw     ax
0041be01  test       ah, 0x41
0041be04  jne        0x41bf06

// block 0041be0a
0041be0a  lea        ebx, [ebx]

// block 0041be10
0041be10  mov        eax, dword ptr [esp + 0x1c]
0041be14  mov        ecx, dword ptr [esp + 0x20]
0041be18  mov        edx, dword ptr [esp + 0x24]
0041be1c  mov        dword ptr [edi + 0x131c], eax
0041be22  mov        dword ptr [edi + 0x1320], ecx
0041be28  mov        dword ptr [edi + 0x1324], edx
0041be2e  fld        dword ptr [ebx + 0x68]
0041be31  mov        esi, 0x4ce568
0041be36  fstp       dword ptr [esp + 0x18]
0041be3a  call       0x464440

// block 0041be3f
0041be3f  mov        dword ptr [esp + 0x14], eax
0041be43  fild       dword ptr [esp + 0x14]
0041be47  test       eax, eax
0041be49  jge        0x41be51

// block 0041be4b
0041be4b  fadd       dword ptr [0x4a3e10]

// block 0041be51
0041be51  fmul       qword ptr [0x4a3ea8]
0041be57  mov        esi, 0x4ce568
0041be5c  fsub       qword ptr [0x4a3ce0]
0041be62  fstp       dword ptr [esp + 0x14]
0041be66  fld        dword ptr [esp + 0x14]
0041be6a  fmul       qword ptr [0x4a3cd8]
0041be70  fstp       dword ptr [esp + 0x14]
0041be74  fld        dword ptr [esp + 0x14]
0041be78  fdiv       qword ptr [0x4a4280]
0041be7e  fadd       dword ptr [esp + 0x18]
0041be82  fstp       dword ptr [edi + 0x1328]
0041be88  call       0x464440

// block 0041be8d
0041be8d  mov        dword ptr [esp + 0x18], eax
0041be91  fild       dword ptr [esp + 0x18]
0041be95  test       eax, eax
0041be97  jge        0x41be9f

// block 0041be99
0041be99  fadd       dword ptr [0x4a3e10]

// block 0041be9f
0041be9f  fmul       qword ptr [0x4a3eb0]
0041bea5  mov        esi, ebp
0041bea7  fstp       dword ptr [esp + 0x18]
0041beab  fld        dword ptr [edi + 0x250]
0041beb1  fadd       dword ptr [esp + 0x18]
0041beb5  fstp       dword ptr [edi + 0x1330]
0041bebb  fld        dword ptr [esp + 0x28]
0041bebf  fadd       dword ptr [esp + 0x1c]
0041bec3  fstp       dword ptr [esp + 0x1c]
0041bec7  fld        dword ptr [esp + 0x2c]
0041becb  fadd       dword ptr [esp + 0x20]
0041becf  fstp       dword ptr [esp + 0x20]
0041bed3  fld        dword ptr [esp + 0x30]
0041bed7  fadd       dword ptr [esp + 0x24]
0041bedb  fstp       dword ptr [esp + 0x24]
0041bedf  call       0x40b5e0

// block 0041bee4
0041bee4  fld        dword ptr [edi + 0x24c]
0041beea  fadd       dword ptr [esp + 0x10]
0041beee  fstp       dword ptr [esp + 0x10]
0041bef2  fld        dword ptr [esp + 0x10]
0041bef6  fld        dword ptr [ebx + 0x6c]
0041bef9  fcompp     
0041befb  fnstsw     ax
0041befd  test       ah, 0x41
0041bf00  je         0x41be10

// block 0041bf06
0041bf06  mov        eax, dword ptr [ebx]
0041bf08  mov        edx, dword ptr [eax + 0x14]
0041bf0b  push       0
0041bf0d  push       0
0041bf0f  mov        ecx, ebx
0041bf11  call       edx

// block 0041bf13
0041bf13  mov        eax, dword ptr [0x4b44f4]

// block 0041bf18
0041bf18  mov        ecx, dword ptr [eax + 0x48c]
0041bf1e  test       ecx, ecx
0041bf20  je         0x41bf33

// block 0041bf22
0041bf22  mov        ebx, dword ptr [ecx + 8]
0041bf25  mov        dword ptr [eax + 0x48c], ebx
0041bf2b  test       ebx, ebx
0041bf2d  jne        0x41bdb0

// block 0041bf33
0041bf33  pop        esi
0041bf34  pop        edi
0041bf35  pop        ebp
0041bf36  xor        eax, eax
0041bf38  pop        ebx
0041bf39  add        esp, 0x24
0041bf3c  ret        

// block 0041bf3d
0041bf3d  pop        edi
0041bf3e  pop        ebp
0041bf3f  xor        eax, eax
0041bf41  pop        ebx
0041bf42  add        esp, 0x24
0041bf45  ret        
