
// block 004270b0
004270b0  sub        esp, 0x14
004270b3  push       ebx
004270b4  push       ebp
004270b5  mov        ebp, dword ptr [0x4b0c58]
004270bb  cmp        ebp, 3
004270be  push       esi
004270bf  push       edi
004270c0  mov        esi, eax
004270c2  mov        ebx, ecx
004270c4  jl         0x427105

// block 004270c6
004270c6  mov        eax, dword ptr [0x4b0c78]
004270cb  mov        ecx, dword ptr [0x4b0cf4]
004270d1  add        eax, 0x186a0
004270d6  cmp        eax, ecx
004270d8  mov        dword ptr [0x4b0c78], eax
004270dd  jle        0x4270e5

// block 004270df
004270df  mov        dword ptr [0x4b0c78], ecx

// block 004270e5
004270e5  push       0xff80ff80
004270ea  add        ebx, 0x96c
004270f0  push       ebx
004270f1  mov        eax, 0x3e8
004270f6  call       0x43e250

// block 004270fb
004270fb  xor        eax, eax
004270fd  pop        edi
004270fe  pop        esi
004270ff  pop        ebp
00427100  pop        ebx
00427101  add        esp, 0x14
00427104  ret        

// block 00427105
00427105  mov        edx, dword ptr [0x4b43c8]
0042710b  fld        dword ptr [0x4a4368]
00427111  mov        eax, 0xfffffffe
00427116  fstp       dword ptr [esp + 0x1c]
0042711a  and        dword ptr [ebx + 0x47c], eax
00427120  fldz       
00427122  and        dword ptr [ebx + 0x930], eax
00427128  fstp       dword ptr [esp + 0x20]
0042712c  push       0
0042712e  lea        eax, [esi + 0xc9]
00427134  push       eax
00427135  mov        eax, dword ptr [edx + 0x4debdc]
0042713b  lea        ecx, [esp + 0x1c]
0042713f  push       ecx
00427140  push       eax
00427141  mov        eax, 0x17
00427146  xor        ecx, ecx
00427148  call       0x4615a0

// block 0042714d
0042714d  lea        edi, [esi + 1]
00427150  mov        esi, 0x4b0c4c
00427155  call       0x422e80

// block 0042715a
0042715a  cmp        dword ptr [0x4b0c5c], 4
00427161  jne        0x427164

// block 00427163
00427163  dec        ebp

// block 00427164
00427164  mov        edx, dword ptr [esp + 0x14]
00427168  mov        ecx, ebp
0042716a  imul       ecx, ecx, 0x15
0042716d  mov        dword ptr [esp + 0x10], ecx
00427171  push       4
00427173  push       0xf
00427175  fild       dword ptr [esp + 0x18]
00427179  push       edx
0042717a  mov        edx, dword ptr [0x4ce8cc]
00427180  lea        esi, [ebx + 0x424]
00427186  fadd       qword ptr [0x4a4360]
0042718c  fstp       dword ptr [esp + 0x24]
00427190  call       0x461920

// block 00427195
00427195  lea        ecx, [esp + 0x20]
00427199  mov        edx, esi
0042719b  call       0x427b90

// block 004271a0
004271a0  mov        eax, dword ptr [0x4b43e4]
004271a5  push       ebp
004271a6  push       eax
004271a7  call       0x4214b0

// block 004271ac
004271ac  pop        edi
004271ad  pop        esi
004271ae  pop        ebp
004271af  xor        eax, eax
004271b1  pop        ebx
004271b2  add        esp, 0x14
004271b5  ret        
