
// block 00460800
00460800  sub        esp, 0x8c
00460806  mov        eax, dword ptr [0x4ad138]
0046080b  xor        eax, esp
0046080d  mov        dword ptr [esp + 0x88], eax
00460814  mov        al, byte ptr [esi + 0x49c]
0046081a  push       ebx
0046081b  push       ebp
0046081c  push       edi
0046081d  mov        ebp, 0x11
00460822  test       al, al
00460824  jbe        0x460829

// block 00460826
00460826  movzx      ebp, al

// block 00460829
00460829  mov        ecx, dword ptr [esp + 0xac]
00460830  lea        eax, [esp + 0xb0]
00460837  push       eax
00460838  push       ecx
00460839  lea        edx, [esp + 0x18]
0046083d  push       edx
0046083e  call       0x46cad8

// block 00460843
00460843  lea        eax, [esp + 0x1c]
00460847  add        esp, 0xc
0046084a  lea        edx, [eax + 1]
0046084d  lea        ecx, [ecx]

// block 00460850
00460850  mov        cl, byte ptr [eax]
00460852  inc        eax
00460853  test       cl, cl
00460855  jne        0x460850

// block 00460857
00460857  mov        ecx, dword ptr [esp + 0xa8]
0046085e  mov        edi, dword ptr [esi + 0x3f4]
00460864  fld        dword ptr [edi + 0x38]
00460867  sub        eax, edx
00460869  mov        edx, dword ptr [esi + 0x480]
0046086f  push       ecx
00460870  mov        ecx, dword ptr [esp + 0xa4]
00460877  shr        edx, 3
0046087a  and        edx, 1
0046087d  push       edx
0046087e  mov        edx, dword ptr [esp + 0xa4]
00460885  push       ecx
00460886  lea        ecx, [ebp - 1]
00460889  imul       ecx, eax
0046088c  shr        ecx, 1
0046088e  mov        dword ptr [esp + 0x18], ecx
00460892  push       edx
00460893  fild       dword ptr [esp + 0x1c]
00460897  test       ecx, ecx
00460899  jge        0x4608a1

// block 0046089b
0046089b  fadd       dword ptr [0x4a3e10]

// block 004608a1
004608a1  fsubp      st(1)
004608a3  call       0x4931e0

// block 004608a8
004608a8  mov        edx, dword ptr [edi + 8]
004608ab  push       eax
004608ac  mov        eax, dword ptr [edx]
004608ae  push       edi
004608af  mov        edi, dword ptr [esp + 0xbc]
004608b6  push       eax
004608b7  lea        ebx, [esp + 0x2c]
004608bb  mov        eax, ebp
004608bd  call       0x4606b0

// block 004608c2
004608c2  mov        ecx, dword ptr [esp + 0x94]
004608c9  or         dword ptr [esi + 0x47c], 1
004608d0  pop        edi
004608d1  pop        ebp
004608d2  pop        ebx
004608d3  xor        ecx, esp
004608d5  call       0x46c932

// block 004608da
004608da  add        esp, 0x8c
004608e0  ret        
