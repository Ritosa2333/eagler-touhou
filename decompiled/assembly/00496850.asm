
// block 00496850
00496850  mov        edi, edi
00496852  push       ebx
00496853  mov        ebx, esp
00496855  push       ecx
00496856  push       ecx
00496857  and        esp, 0xfffffff0
0049685a  add        esp, 4
0049685d  push       ebp
0049685e  mov        ebp, dword ptr [ebx + 4]
00496861  mov        dword ptr [esp + 4], ebp
00496865  mov        ebp, esp
00496867  sub        esp, 0x80
0049686d  mov        eax, dword ptr [0x4ad138]
00496872  xor        eax, ebp
00496874  mov        dword ptr [ebp - 4], eax
00496877  push       dword ptr [ebx + 0x20]
0049687a  lea        eax, [ebx + 0x18]
0049687d  push       eax
0049687e  push       dword ptr [ebx + 8]
00496881  call       0x496491

// block 00496886
00496886  add        esp, 0xc
00496889  test       eax, eax
0049688b  jne        0x4968af

// block 0049688d
0049688d  and        dword ptr [ebp - 0x40], 0xfffffffe
00496891  lea        eax, [ebx + 0x18]
00496894  push       eax
00496895  lea        eax, [ebx + 0x10]
00496898  push       eax
00496899  push       dword ptr [ebx + 0xc]
0049689c  lea        eax, [ebx + 0x20]
0049689f  push       dword ptr [ebx + 8]
004968a2  push       eax
004968a3  lea        eax, [ebp - 0x80]
004968a6  push       eax
004968a7  call       0x49644b

// block 004968ac
004968ac  add        esp, 0x18

// block 004968af
004968af  push       dword ptr [ebx + 8]
004968b2  call       0x4966c6

// block 004968b7
004968b7  add        esp, 4
004968ba  cmp        dword ptr [0x4b37a0], 0
004968c1  jne        0x4968ee

// block 004968c3
004968c3  test       eax, eax
004968c5  je         0x4968ee

// block 004968c7
004968c7  push       dword ptr [ebx + 0x20]
004968ca  fld        qword ptr [ebx + 0x18]
004968cd  sub        esp, 0x18
004968d0  fstp       qword ptr [esp + 0x10]
004968d4  fldz       
004968d6  fstp       qword ptr [esp + 8]
004968da  fld        qword ptr [ebx + 0x10]
004968dd  fstp       qword ptr [esp]
004968e0  push       dword ptr [ebx + 0xc]
004968e3  push       eax
004968e4  call       0x4966fa

// block 004968e9
004968e9  add        esp, 0x24
004968ec  jmp        0x496908

// block 004968ee
004968ee  push       eax
004968ef  call       0x496673

// block 004968f4
004968f4  mov        dword ptr [esp], 0xffff
004968fb  push       dword ptr [ebx + 0x20]
004968fe  call       0x491181

// block 00496903
00496903  fld        qword ptr [ebx + 0x18]
00496906  pop        ecx
00496907  pop        ecx

// block 00496908
00496908  mov        ecx, dword ptr [ebp - 4]
0049690b  xor        ecx, ebp
0049690d  call       0x46c932

// block 00496912
00496912  mov        esp, ebp
00496914  pop        ebp
00496915  mov        esp, ebx
00496917  pop        ebx
00496918  ret        
