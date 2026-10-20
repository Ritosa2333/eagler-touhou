
// block 004608f0
004608f0  sub        esp, 0x8c
004608f6  mov        eax, dword ptr [0x4ad138]
004608fb  xor        eax, esp
004608fd  mov        dword ptr [esp + 0x88], eax
00460904  mov        al, byte ptr [esi + 0x49c]
0046090a  push       ebx
0046090b  push       ebp
0046090c  push       edi
0046090d  mov        ebp, 0x11
00460912  test       al, al
00460914  jbe        0x460919

// block 00460916
00460916  movzx      ebp, al

// block 00460919
00460919  mov        ecx, dword ptr [esp + 0xac]
00460920  lea        eax, [esp + 0xb0]
00460927  push       eax
00460928  push       ecx
00460929  lea        edx, [esp + 0x18]
0046092d  push       edx
0046092e  call       0x46cad8

// block 00460933
00460933  lea        eax, [esp + 0x1c]
00460937  add        esp, 0xc
0046093a  lea        edx, [eax + 1]
0046093d  lea        ecx, [ecx]

// block 00460940
00460940  mov        cl, byte ptr [eax]
00460942  inc        eax
00460943  test       cl, cl
00460945  jne        0x460940

// block 00460947
00460947  mov        ecx, dword ptr [esp + 0xa8]
0046094e  mov        edi, dword ptr [esi + 0x3f4]
00460954  fld        dword ptr [edi + 0x38]
00460957  sub        eax, edx
00460959  mov        edx, dword ptr [esi + 0x480]
0046095f  lea        ebx, [ebp - 1]
00460962  imul       ebx, eax
00460965  push       ecx
00460966  mov        ecx, dword ptr [esp + 0xa4]
0046096d  shr        edx, 3
00460970  and        edx, 1
00460973  push       edx
00460974  mov        edx, dword ptr [esp + 0xa4]
0046097b  push       ecx
0046097c  push       edx
0046097d  shr        ebx, 2
00460980  call       0x4931e0

// block 00460985
00460985  cdq        
00460986  sub        eax, edx
00460988  sar        eax, 1
0046098a  sub        eax, ebx
0046098c  push       eax
0046098d  mov        eax, dword ptr [edi + 8]
00460990  mov        ecx, dword ptr [eax]
00460992  push       edi
00460993  mov        edi, dword ptr [esp + 0xbc]
0046099a  push       ecx
0046099b  lea        ebx, [esp + 0x2c]
0046099f  mov        eax, ebp
004609a1  call       0x4606b0

// block 004609a6
004609a6  mov        ecx, dword ptr [esp + 0x94]
004609ad  or         dword ptr [esi + 0x47c], 1
004609b4  pop        edi
004609b5  pop        ebp
004609b6  pop        ebx
004609b7  xor        ecx, esp
004609b9  call       0x46c932

// block 004609be
004609be  add        esp, 0x8c
004609c4  ret        
