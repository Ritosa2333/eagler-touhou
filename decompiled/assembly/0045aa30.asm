
// block 0045aa30
0045aa30  sub        esp, 0x20
0045aa33  mov        eax, dword ptr [esp + 0x24]
0045aa37  cmp        dword ptr [eax + 0x3c], 0
0045aa3b  fld        dword ptr [eax + 0x58]
0045aa3e  fmul       dword ptr [eax + 0x40]
0045aa41  fstp       dword ptr [esp + 0x24]
0045aa45  fld        dword ptr [eax + 0x5c]
0045aa48  fmul       dword ptr [eax + 0x44]
0045aa4b  fstp       dword ptr [esp]
0045aa4e  je         0x45aa67

// block 0045aa50
0045aa50  mov        ecx, dword ptr [eax + 0x3c]
0045aa53  fld        dword ptr [ecx + 0x40]
0045aa56  fmul       dword ptr [esp + 0x24]
0045aa5a  fstp       dword ptr [esp + 0x24]
0045aa5e  fld        dword ptr [ecx + 0x44]
0045aa61  fmul       dword ptr [esp]
0045aa64  fstp       dword ptr [esp]

// block 0045aa67
0045aa67  fld        dword ptr [esp]
0045aa6a  mov        ecx, dword ptr [eax + 0x47c]
0045aa70  fld        st(0)
0045aa72  shr        ecx, 0x13
0045aa75  fld        qword ptr [0x4a3cf8]
0045aa7b  and        ecx, 3
0045aa7e  sub        ecx, 0
0045aa81  fmul       st(1), st(0)
0045aa83  fxch       st(1)
0045aa85  fstp       dword ptr [esp + 4]
0045aa89  je         0x45ab19

// block 0045aa8f
0045aa8f  sub        ecx, 1
0045aa92  fstp       st(0)
0045aa94  je         0x45aadd

// block 0045aa96
0045aa96  sub        ecx, 1
0045aa99  jne        0x45ab57

// block 0045aa9f
0045aa9f  fld        dword ptr [eax + 0x430]
0045aaa5  fadd       dword ptr [eax + 0x424]
0045aaab  fadd       dword ptr [eax + 0x43c]
0045aab1  fsub       dword ptr [esp + 0x24]
0045aab5  fstp       dword ptr [esp + 0x24]
0045aab9  fld        dword ptr [esp + 0x24]
0045aabd  fst        dword ptr [esi]
0045aabf  fstp       dword ptr [ebx]
0045aac1  fld        dword ptr [eax + 0x430]
0045aac7  fadd       dword ptr [eax + 0x424]
0045aacd  fadd       dword ptr [eax + 0x43c]
0045aad3  fstp       dword ptr [esp + 0x24]
0045aad7  fld        dword ptr [esp + 0x24]
0045aadb  jmp        0x45ab53

// block 0045aadd
0045aadd  fld        dword ptr [eax + 0x430]
0045aae3  fadd       dword ptr [eax + 0x424]
0045aae9  fadd       dword ptr [eax + 0x43c]
0045aaef  fstp       dword ptr [esp]
0045aaf2  fld        dword ptr [esp]
0045aaf5  fst        dword ptr [esi]
0045aaf7  fstp       dword ptr [ebx]
0045aaf9  fld        dword ptr [eax + 0x430]
0045aaff  fadd       dword ptr [eax + 0x424]
0045ab05  fadd       dword ptr [eax + 0x43c]
0045ab0b  fadd       dword ptr [esp + 0x24]
0045ab0f  fstp       dword ptr [esp + 0x24]
0045ab13  fld        dword ptr [esp + 0x24]
0045ab17  jmp        0x45ab53

// block 0045ab19
0045ab19  fld        dword ptr [eax + 0x430]
0045ab1f  fadd       dword ptr [eax + 0x424]
0045ab25  fadd       dword ptr [eax + 0x43c]
0045ab2b  fld        dword ptr [esp + 0x24]
0045ab2f  fld        st(0)
0045ab31  fmulp      st(3)
0045ab33  fxch       st(2)
0045ab35  fstp       dword ptr [esp + 0x24]
0045ab39  fsub       dword ptr [esp + 0x24]
0045ab3d  fstp       dword ptr [esp + 0x24]
0045ab41  fld        dword ptr [esp + 0x24]
0045ab45  fst        dword ptr [esi]
0045ab47  fst        dword ptr [ebx]
0045ab49  faddp      st(1)
0045ab4b  fstp       dword ptr [esp + 0x24]
0045ab4f  fld        dword ptr [esp + 0x24]

// block 0045ab53
0045ab53  fst        dword ptr [edx]
0045ab55  fstp       dword ptr [edi]

// block 0045ab57
0045ab57  mov        ecx, dword ptr [eax + 0x47c]
0045ab5d  shr        ecx, 0x15
0045ab60  and        ecx, 3
0045ab63  sub        ecx, 0
0045ab66  je         0x45ac02

// block 0045ab6c
0045ab6c  sub        ecx, 1
0045ab6f  je         0x45abbe

// block 0045ab71
0045ab71  sub        ecx, 1
0045ab74  jne        0x45ac38

// block 0045ab7a
0045ab7a  fld        dword ptr [eax + 0x434]
0045ab80  fadd       dword ptr [eax + 0x428]
0045ab86  fadd       dword ptr [eax + 0x440]
0045ab8c  fsubrp     st(1)
0045ab8e  fstp       dword ptr [esp + 0x24]
0045ab92  fld        dword ptr [esp + 0x24]
0045ab96  fst        dword ptr [edi + 4]
0045ab99  fstp       dword ptr [ebx + 4]
0045ab9c  fld        dword ptr [eax + 0x434]
0045aba2  fadd       dword ptr [eax + 0x428]
0045aba8  fadd       dword ptr [eax + 0x440]
0045abae  fstp       dword ptr [esp + 0x24]
0045abb2  fld        dword ptr [esp + 0x24]
0045abb6  fst        dword ptr [edx + 4]
0045abb9  fstp       dword ptr [esi + 4]
0045abbc  jmp        0x45ac3a

// block 0045abbe
0045abbe  fld        dword ptr [eax + 0x434]
0045abc4  fadd       dword ptr [eax + 0x428]
0045abca  fadd       dword ptr [eax + 0x440]
0045abd0  fstp       dword ptr [esp + 0x24]
0045abd4  fld        dword ptr [esp + 0x24]
0045abd8  fst        dword ptr [edi + 4]
0045abdb  fstp       dword ptr [ebx + 4]
0045abde  fld        dword ptr [eax + 0x434]
0045abe4  fadd       dword ptr [eax + 0x428]
0045abea  fadd       dword ptr [eax + 0x440]
0045abf0  faddp      st(1)
0045abf2  fstp       dword ptr [esp + 0x24]
0045abf6  fld        dword ptr [esp + 0x24]
0045abfa  fst        dword ptr [edx + 4]
0045abfd  fstp       dword ptr [esi + 4]
0045ac00  jmp        0x45ac3a

// block 0045ac02
0045ac02  fld        dword ptr [eax + 0x434]
0045ac08  fadd       dword ptr [eax + 0x428]
0045ac0e  fadd       dword ptr [eax + 0x440]
0045ac14  fsub       dword ptr [esp + 4]
0045ac18  fstp       dword ptr [esp + 0x24]
0045ac1c  fld        dword ptr [esp + 0x24]
0045ac20  fst        dword ptr [edi + 4]
0045ac23  fst        dword ptr [ebx + 4]
0045ac26  faddp      st(1)
0045ac28  fstp       dword ptr [esp + 0x24]
0045ac2c  fld        dword ptr [esp + 0x24]
0045ac30  fst        dword ptr [edx + 4]
0045ac33  fstp       dword ptr [esi + 4]
0045ac36  jmp        0x45ac3a

// block 0045ac38
0045ac38  fstp       st(0)

// block 0045ac3a
0045ac3a  cmp        dword ptr [eax + 0x3c], 0
0045ac3e  je         0x45ae15

// block 0045ac44
0045ac44  mov        ecx, dword ptr [eax + 0x3c]
0045ac47  fld        dword ptr [ecx + 0x424]
0045ac4d  fadd       dword ptr [ecx + 0x430]
0045ac53  fstp       dword ptr [esp + 8]
0045ac57  fld        dword ptr [ecx + 0x428]
0045ac5d  fadd       dword ptr [ecx + 0x434]
0045ac63  fstp       dword ptr [esp + 0xc]
0045ac67  fld        dword ptr [ecx + 0x42c]
0045ac6d  fadd       dword ptr [ecx + 0x438]
0045ac73  fstp       dword ptr [esp + 0x10]
0045ac77  fld        dword ptr [ecx + 0x43c]
0045ac7d  fadd       dword ptr [esp + 8]
0045ac81  fstp       dword ptr [esp + 0x14]
0045ac85  fld        dword ptr [ecx + 0x440]
0045ac8b  fadd       dword ptr [esp + 0xc]
0045ac8f  fstp       dword ptr [esp + 0x18]
0045ac93  fld        dword ptr [ecx + 0x444]
0045ac99  fadd       dword ptr [esp + 0x10]
0045ac9d  fstp       dword ptr [esp + 0x1c]
0045aca1  fld        dword ptr [ebx]
0045aca3  fadd       dword ptr [esp + 0x14]
0045aca7  fstp       dword ptr [ebx]
0045aca9  fld        dword ptr [esp + 0x18]
0045acad  fadd       dword ptr [ebx + 4]
0045acb0  fstp       dword ptr [ebx + 4]
0045acb3  fld        dword ptr [esp + 0x1c]
0045acb7  fadd       dword ptr [ebx + 8]
0045acba  fstp       dword ptr [ebx + 8]
0045acbd  mov        ecx, dword ptr [eax + 0x3c]
0045acc0  fld        dword ptr [ecx + 0x430]
0045acc6  fadd       dword ptr [ecx + 0x424]
0045accc  fstp       dword ptr [esp + 0x14]
0045acd0  fld        dword ptr [ecx + 0x428]
0045acd6  fadd       dword ptr [ecx + 0x434]
0045acdc  fstp       dword ptr [esp + 0x18]
0045ace0  fld        dword ptr [ecx + 0x42c]
0045ace6  fadd       dword ptr [ecx + 0x438]
0045acec  fstp       dword ptr [esp + 0x1c]
0045acf0  fld        dword ptr [esp + 0x14]
0045acf4  fadd       dword ptr [ecx + 0x43c]
0045acfa  fstp       dword ptr [esp + 8]
0045acfe  fld        dword ptr [ecx + 0x440]
0045ad04  fadd       dword ptr [esp + 0x18]
0045ad08  fstp       dword ptr [esp + 0xc]
0045ad0c  fld        dword ptr [ecx + 0x444]
0045ad12  fadd       dword ptr [esp + 0x1c]
0045ad16  fstp       dword ptr [esp + 0x10]
0045ad1a  fld        dword ptr [edi]
0045ad1c  fadd       dword ptr [esp + 8]
0045ad20  fstp       dword ptr [edi]
0045ad22  fld        dword ptr [edi + 4]
0045ad25  fadd       dword ptr [esp + 0xc]
0045ad29  fstp       dword ptr [edi + 4]
0045ad2c  fld        dword ptr [esp + 0x10]
0045ad30  fadd       dword ptr [edi + 8]
0045ad33  fstp       dword ptr [edi + 8]
0045ad36  mov        ecx, dword ptr [eax + 0x3c]
0045ad39  fld        dword ptr [ecx + 0x424]
0045ad3f  fadd       dword ptr [ecx + 0x430]
0045ad45  fstp       dword ptr [esp + 0x14]
0045ad49  fld        dword ptr [ecx + 0x428]
0045ad4f  fadd       dword ptr [ecx + 0x434]
0045ad55  fstp       dword ptr [esp + 0x18]
0045ad59  fld        dword ptr [ecx + 0x42c]
0045ad5f  fadd       dword ptr [ecx + 0x438]
0045ad65  fstp       dword ptr [esp + 0x1c]
0045ad69  fld        dword ptr [ecx + 0x43c]
0045ad6f  fadd       dword ptr [esp + 0x14]
0045ad73  fstp       dword ptr [esp + 8]
0045ad77  fld        dword ptr [ecx + 0x440]
0045ad7d  fadd       dword ptr [esp + 0x18]
0045ad81  fstp       dword ptr [esp + 0xc]
0045ad85  fld        dword ptr [ecx + 0x444]
0045ad8b  fadd       dword ptr [esp + 0x1c]
0045ad8f  fstp       dword ptr [esp + 0x10]
0045ad93  fld        dword ptr [esp + 8]
0045ad97  fadd       dword ptr [esi]
0045ad99  fstp       dword ptr [esi]
0045ad9b  fld        dword ptr [esi + 4]
0045ad9e  fadd       dword ptr [esp + 0xc]
0045ada2  fstp       dword ptr [esi + 4]
0045ada5  fld        dword ptr [esp + 0x10]
0045ada9  fadd       dword ptr [esi + 8]
0045adac  fstp       dword ptr [esi + 8]
0045adaf  mov        ecx, dword ptr [eax + 0x3c]
0045adb2  fld        dword ptr [ecx + 0x430]
0045adb8  add        ecx, 0x43c
0045adbe  fadd       dword ptr [ecx - 0x18]
0045adc1  fstp       dword ptr [esp + 0x14]
0045adc5  fld        dword ptr [ecx - 0x14]
0045adc8  fadd       dword ptr [ecx - 8]
0045adcb  fstp       dword ptr [esp + 0x18]
0045adcf  fld        dword ptr [ecx - 0x10]

// block 0045add2
0045add2  fadd       dword ptr [ecx - 4]
0045add5  fstp       dword ptr [esp + 0x1c]
0045add9  fld        dword ptr [ecx]
0045addb  fadd       dword ptr [esp + 0x14]
0045addf  fstp       dword ptr [esp + 8]
0045ade3  fld        dword ptr [ecx + 4]
0045ade6  fadd       dword ptr [esp + 0x18]
0045adea  fstp       dword ptr [esp + 0xc]
0045adee  fld        dword ptr [ecx + 8]
0045adf1  fadd       dword ptr [esp + 0x1c]
0045adf5  fstp       dword ptr [esp + 0x10]
0045adf9  fld        dword ptr [esp + 8]
0045adfd  fadd       dword ptr [edx]
0045adff  fstp       dword ptr [edx]
0045ae01  fld        dword ptr [esp + 0xc]
0045ae05  fadd       dword ptr [edx + 4]
0045ae08  fstp       dword ptr [edx + 4]
0045ae0b  fld        dword ptr [esp + 0x10]
0045ae0f  fadd       dword ptr [edx + 8]
0045ae12  fstp       dword ptr [edx + 8]

// block 0045ae15
0045ae15  fld        dword ptr [eax + 0x438]
0045ae1b  fadd       dword ptr [eax + 0x42c]
0045ae21  fadd       dword ptr [eax + 0x444]
0045ae27  fstp       dword ptr [esp + 0x24]
0045ae2b  fld        dword ptr [esp + 0x24]
0045ae2f  fst        dword ptr [edx + 8]
0045ae32  fst        dword ptr [esi + 8]
0045ae35  fst        dword ptr [edi + 8]
0045ae38  fstp       dword ptr [ebx + 8]
0045ae3b  add        esp, 0x20
0045ae3e  ret        4
