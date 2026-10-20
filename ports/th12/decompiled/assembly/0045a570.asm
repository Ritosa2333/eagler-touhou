
// block 0045a570
0045a570  push       ebp
0045a571  mov        ebp, esp
0045a573  and        esp, 0xffffffc0
0045a576  sub        esp, 0x40
0045a579  cmp        dword ptr [esi + 0x3c], 0
0045a57d  fld        dword ptr [esi + 0x58]
0045a580  fmul       dword ptr [esi + 0x40]
0045a583  fstp       dword ptr [esp + 0x18]
0045a587  fld        dword ptr [esi + 0x5c]
0045a58a  fmul       dword ptr [esi + 0x44]
0045a58d  fstp       dword ptr [esp + 0x1c]
0045a591  je         0x45a5ac

// block 0045a593
0045a593  mov        eax, dword ptr [esi + 0x3c]
0045a596  fld        dword ptr [eax + 0x40]
0045a599  fmul       dword ptr [esp + 0x18]
0045a59d  fstp       dword ptr [esp + 0x18]
0045a5a1  fld        dword ptr [eax + 0x44]
0045a5a4  fmul       dword ptr [esp + 0x1c]
0045a5a8  fstp       dword ptr [esp + 0x1c]

// block 0045a5ac
0045a5ac  fld        dword ptr [esp + 0x1c]
0045a5b0  mov        eax, dword ptr [esi + 0x47c]
0045a5b6  fld        st(0)
0045a5b8  shr        eax, 0x13
0045a5bb  fld        qword ptr [0x4a3cf8]
0045a5c1  and        eax, 3
0045a5c4  sub        eax, 0
0045a5c7  fmul       st(1), st(0)
0045a5c9  fxch       st(1)
0045a5cb  fstp       dword ptr [esp + 0x24]
0045a5cf  je         0x45a677

// block 0045a5d5
0045a5d5  sub        eax, 1
0045a5d8  fstp       st(0)
0045a5da  mov        ecx, dword ptr [ebp + 0xc]
0045a5dd  je         0x45a630

// block 0045a5df
0045a5df  sub        eax, 1
0045a5e2  mov        edx, dword ptr [ebp + 8]
0045a5e5  jne        0x45a6d9

// block 0045a5eb
0045a5eb  fld        dword ptr [esi + 0x430]
0045a5f1  fadd       dword ptr [esi + 0x424]
0045a5f7  fadd       dword ptr [esi + 0x43c]
0045a5fd  fsub       dword ptr [esp + 0x18]
0045a601  fstp       dword ptr [esp + 0x18]
0045a605  fld        dword ptr [esp + 0x18]
0045a609  fst        dword ptr [ebx]
0045a60b  fstp       dword ptr [edx]
0045a60d  fld        dword ptr [esi + 0x430]
0045a613  fadd       dword ptr [esi + 0x424]
0045a619  fadd       dword ptr [esi + 0x43c]
0045a61f  fstp       dword ptr [esp + 0x18]
0045a623  fld        dword ptr [esp + 0x18]
0045a627  fst        dword ptr [edi]
0045a629  fstp       dword ptr [ecx]
0045a62b  jmp        0x45a6d9

// block 0045a630
0045a630  fld        dword ptr [esi + 0x430]
0045a636  mov        eax, dword ptr [ebp + 8]
0045a639  fadd       dword ptr [esi + 0x424]
0045a63f  mov        edx, eax
0045a641  fadd       dword ptr [esi + 0x43c]
0045a647  fstp       dword ptr [esp + 0x20]
0045a64b  fld        dword ptr [esp + 0x20]
0045a64f  fst        dword ptr [ebx]
0045a651  fstp       dword ptr [eax]
0045a653  fld        dword ptr [esi + 0x430]
0045a659  fadd       dword ptr [esi + 0x424]
0045a65f  fadd       dword ptr [esi + 0x43c]
0045a665  fadd       dword ptr [esp + 0x18]
0045a669  fstp       dword ptr [esp + 0x20]
0045a66d  fld        dword ptr [esp + 0x20]
0045a671  fst        dword ptr [edi]
0045a673  fstp       dword ptr [ecx]
0045a675  jmp        0x45a6d9

// block 0045a677
0045a677  fstp       st(1)
0045a679  sub        esp, 8
0045a67c  fld        dword ptr [esi + 0x430]
0045a682  fadd       dword ptr [esi + 0x424]
0045a688  fadd       dword ptr [esi + 0x43c]
0045a68e  fld        dword ptr [esp + 0x20]
0045a692  fmulp      st(2)
0045a694  fxch       st(1)
0045a696  fstp       dword ptr [esp + 0x28]
0045a69a  fsub       dword ptr [esp + 0x28]
0045a69e  fstp       dword ptr [esp + 0x28]
0045a6a2  fld        dword ptr [esp + 0x28]
0045a6a6  fstp       qword ptr [esp]
0045a6a9  call       0x493290

// block 0045a6ae
0045a6ae  mov        edx, dword ptr [ebp + 8]
0045a6b1  fstp       dword ptr [esp + 0x28]
0045a6b5  fld        dword ptr [esp + 0x28]
0045a6b9  mov        eax, dword ptr [ebp + 0xc]
0045a6bc  fst        dword ptr [ebx]
0045a6be  add        esp, 8
0045a6c1  fst        dword ptr [edx]
0045a6c3  mov        ecx, eax
0045a6c5  fadd       dword ptr [esp + 0x18]
0045a6c9  fstp       dword ptr [esp + 0x20]
0045a6cd  fld        dword ptr [esp + 0x20]
0045a6d1  fst        dword ptr [edi]
0045a6d3  fstp       dword ptr [eax]
0045a6d5  fld        dword ptr [esp + 0x1c]

// block 0045a6d9
0045a6d9  mov        eax, dword ptr [esi + 0x47c]
0045a6df  shr        eax, 0x15
0045a6e2  and        eax, 3
0045a6e5  sub        eax, 0
0045a6e8  je         0x45a787

// block 0045a6ee
0045a6ee  sub        eax, 1
0045a6f1  je         0x45a743

// block 0045a6f3
0045a6f3  sub        eax, 1
0045a6f6  jne        0x45a7dd

// block 0045a6fc
0045a6fc  fld        dword ptr [esi + 0x434]
0045a702  fadd       dword ptr [esi + 0x428]
0045a708  fadd       dword ptr [esi + 0x440]
0045a70e  fsubrp     st(1)
0045a710  fstp       dword ptr [esp + 0x20]
0045a714  fld        dword ptr [esp + 0x20]
0045a718  fst        dword ptr [ecx + 4]
0045a71b  fstp       dword ptr [edx + 4]
0045a71e  fld        dword ptr [esi + 0x434]
0045a724  fadd       dword ptr [esi + 0x428]
0045a72a  fadd       dword ptr [esi + 0x440]
0045a730  fstp       dword ptr [esp + 0x20]
0045a734  fld        dword ptr [esp + 0x20]
0045a738  fst        dword ptr [edi + 4]
0045a73b  fstp       dword ptr [ebx + 4]
0045a73e  jmp        0x45a7df

// block 0045a743
0045a743  fld        dword ptr [esi + 0x434]
0045a749  fadd       dword ptr [esi + 0x428]
0045a74f  fadd       dword ptr [esi + 0x440]
0045a755  fstp       dword ptr [esp + 0x20]
0045a759  fld        dword ptr [esp + 0x20]
0045a75d  fst        dword ptr [ecx + 4]
0045a760  fstp       dword ptr [edx + 4]
0045a763  fld        dword ptr [esi + 0x434]
0045a769  fadd       dword ptr [esi + 0x428]
0045a76f  fadd       dword ptr [esi + 0x440]
0045a775  faddp      st(1)
0045a777  fstp       dword ptr [esp + 0x20]
0045a77b  fld        dword ptr [esp + 0x20]
0045a77f  fst        dword ptr [edi + 4]
0045a782  fstp       dword ptr [ebx + 4]
0045a785  jmp        0x45a7df

// block 0045a787
0045a787  fstp       st(0)
0045a789  sub        esp, 8
0045a78c  fld        dword ptr [esi + 0x434]
0045a792  fadd       dword ptr [esi + 0x428]
0045a798  fadd       dword ptr [esi + 0x440]
0045a79e  fsub       dword ptr [esp + 0x2c]
0045a7a2  fstp       dword ptr [esp + 0x2c]
0045a7a6  fld        dword ptr [esp + 0x2c]
0045a7aa  fstp       qword ptr [esp]
0045a7ad  call       0x493290

// block 0045a7b2
0045a7b2  mov        ecx, dword ptr [ebp + 0xc]
0045a7b5  fstp       dword ptr [esp + 0x2c]
0045a7b9  fld        dword ptr [esp + 0x2c]
0045a7bd  mov        edx, dword ptr [ebp + 8]
0045a7c0  fst        dword ptr [ecx + 4]
0045a7c3  add        esp, 8
0045a7c6  fst        dword ptr [edx + 4]
0045a7c9  fadd       dword ptr [esp + 0x1c]
0045a7cd  fstp       dword ptr [esp + 0x24]
0045a7d1  fld        dword ptr [esp + 0x24]
0045a7d5  fst        dword ptr [edi + 4]
0045a7d8  fstp       dword ptr [ebx + 4]
0045a7db  jmp        0x45a7df

// block 0045a7dd
0045a7dd  fstp       st(0)

// block 0045a7df
0045a7df  cmp        dword ptr [esi + 0x3c], 0
0045a7e3  je         0x45a9b9

// block 0045a7e9
0045a7e9  mov        eax, dword ptr [esi + 0x3c]
0045a7ec  fld        dword ptr [eax + 0x424]
0045a7f2  fadd       dword ptr [eax + 0x430]
0045a7f8  fstp       dword ptr [esp + 0x28]
0045a7fc  fld        dword ptr [eax + 0x428]
0045a802  fadd       dword ptr [eax + 0x434]
0045a808  fstp       dword ptr [esp + 0x2c]
0045a80c  fld        dword ptr [eax + 0x42c]
0045a812  fadd       dword ptr [eax + 0x438]
0045a818  fstp       dword ptr [esp + 0x30]
0045a81c  fld        dword ptr [esp + 0x28]
0045a820  fadd       dword ptr [eax + 0x43c]
0045a826  fstp       dword ptr [esp + 0x34]
0045a82a  fld        dword ptr [eax + 0x440]
0045a830  fadd       dword ptr [esp + 0x2c]
0045a834  fstp       dword ptr [esp + 0x38]
0045a838  fld        dword ptr [eax + 0x444]
0045a83e  fadd       dword ptr [esp + 0x30]
0045a842  fstp       dword ptr [esp + 0x3c]
0045a846  fld        dword ptr [edx]
0045a848  fadd       dword ptr [esp + 0x34]
0045a84c  fstp       dword ptr [edx]
0045a84e  fld        dword ptr [edx + 4]
0045a851  fadd       dword ptr [esp + 0x38]
0045a855  fstp       dword ptr [edx + 4]
0045a858  fld        dword ptr [edx + 8]
0045a85b  fadd       dword ptr [esp + 0x3c]
0045a85f  fstp       dword ptr [edx + 8]
0045a862  mov        eax, dword ptr [esi + 0x3c]
0045a865  fld        dword ptr [eax + 0x430]
0045a86b  fadd       dword ptr [eax + 0x424]
0045a871  fstp       dword ptr [esp + 0x34]
0045a875  fld        dword ptr [eax + 0x428]
0045a87b  fadd       dword ptr [eax + 0x434]
0045a881  fstp       dword ptr [esp + 0x38]
0045a885  fld        dword ptr [eax + 0x42c]
0045a88b  fadd       dword ptr [eax + 0x438]
0045a891  fstp       dword ptr [esp + 0x3c]
0045a895  fld        dword ptr [esp + 0x34]
0045a899  fadd       dword ptr [eax + 0x43c]
0045a89f  fstp       dword ptr [esp + 0x28]
0045a8a3  fld        dword ptr [eax + 0x440]
0045a8a9  fadd       dword ptr [esp + 0x38]
0045a8ad  fstp       dword ptr [esp + 0x2c]
0045a8b1  fld        dword ptr [eax + 0x444]
0045a8b7  fadd       dword ptr [esp + 0x3c]
0045a8bb  fstp       dword ptr [esp + 0x30]
0045a8bf  fld        dword ptr [ecx]
0045a8c1  fadd       dword ptr [esp + 0x28]
0045a8c5  fstp       dword ptr [ecx]
0045a8c7  fld        dword ptr [esp + 0x2c]
0045a8cb  fadd       dword ptr [ecx + 4]
0045a8ce  fstp       dword ptr [ecx + 4]
0045a8d1  fld        dword ptr [ecx + 8]
0045a8d4  fadd       dword ptr [esp + 0x30]
0045a8d8  fstp       dword ptr [ecx + 8]
0045a8db  mov        eax, dword ptr [esi + 0x3c]
0045a8de  fld        dword ptr [eax + 0x430]
0045a8e4  fadd       dword ptr [eax + 0x424]
0045a8ea  fstp       dword ptr [esp + 0x34]
0045a8ee  fld        dword ptr [eax + 0x428]
0045a8f4  fadd       dword ptr [eax + 0x434]
0045a8fa  fstp       dword ptr [esp + 0x38]
0045a8fe  fld        dword ptr [eax + 0x42c]
0045a904  fadd       dword ptr [eax + 0x438]
0045a90a  fstp       dword ptr [esp + 0x3c]
0045a90e  fld        dword ptr [eax + 0x43c]
0045a914  fadd       dword ptr [esp + 0x34]
0045a918  fstp       dword ptr [esp + 0x28]
0045a91c  fld        dword ptr [eax + 0x440]
0045a922  fadd       dword ptr [esp + 0x38]
0045a926  fstp       dword ptr [esp + 0x2c]
0045a92a  fld        dword ptr [eax + 0x444]
0045a930  fadd       dword ptr [esp + 0x3c]
0045a934  fstp       dword ptr [esp + 0x30]
0045a938  fld        dword ptr [esp + 0x28]
0045a93c  fadd       dword ptr [ebx]
0045a93e  fstp       dword ptr [ebx]
0045a940  fld        dword ptr [esp + 0x2c]
0045a944  fadd       dword ptr [ebx + 4]
0045a947  fstp       dword ptr [ebx + 4]
0045a94a  fld        dword ptr [ebx + 8]
0045a94d  fadd       dword ptr [esp + 0x30]
0045a951  fstp       dword ptr [ebx + 8]
0045a954  mov        eax, dword ptr [esi + 0x3c]
0045a957  fld        dword ptr [eax + 0x424]
0045a95d  add        eax, 0x43c
0045a962  fadd       dword ptr [eax - 0xc]
0045a965  fstp       dword ptr [esp + 0x34]
0045a969  fld        dword ptr [eax - 0x14]
0045a96c  fadd       dword ptr [eax - 8]
0045a96f  fstp       dword ptr [esp + 0x38]
0045a973  fld        dword ptr [eax - 0x10]
0045a976  fadd       dword ptr [eax - 4]

// block 0045a979
0045a979  fstp       dword ptr [esp + 0x3c]
0045a97d  fld        dword ptr [esp + 0x34]
0045a981  fadd       dword ptr [eax]
0045a983  fstp       dword ptr [esp + 0x28]
0045a987  fld        dword ptr [eax + 4]
0045a98a  fadd       dword ptr [esp + 0x38]
0045a98e  fstp       dword ptr [esp + 0x2c]
0045a992  fld        dword ptr [eax + 8]
0045a995  fadd       dword ptr [esp + 0x3c]
0045a999  fstp       dword ptr [esp + 0x30]
0045a99d  fld        dword ptr [esp + 0x28]
0045a9a1  fadd       dword ptr [edi]
0045a9a3  fstp       dword ptr [edi]
0045a9a5  fld        dword ptr [esp + 0x2c]
0045a9a9  fadd       dword ptr [edi + 4]
0045a9ac  fstp       dword ptr [edi + 4]
0045a9af  fld        dword ptr [edi + 8]
0045a9b2  fadd       dword ptr [esp + 0x30]
0045a9b6  fstp       dword ptr [edi + 8]

// block 0045a9b9
0045a9b9  fld        dword ptr [esi + 0x438]
0045a9bf  fadd       dword ptr [esi + 0x42c]
0045a9c5  fadd       dword ptr [esi + 0x444]
0045a9cb  fstp       dword ptr [esp + 0x24]
0045a9cf  fld        dword ptr [esp + 0x24]
0045a9d3  fst        dword ptr [edi + 8]
0045a9d6  fst        dword ptr [ebx + 8]
0045a9d9  fst        dword ptr [ecx + 8]
0045a9dc  fstp       dword ptr [edx + 8]
0045a9df  mov        esp, ebp
0045a9e1  pop        ebp
0045a9e2  ret        8
