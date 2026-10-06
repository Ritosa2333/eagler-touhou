
// block 00495fde
00495fde  pextrw     eax, xmm0, 3
00495fe3  and        ax, 0x7fff
00495fe7  sub        ax, 0x3030
00495feb  cmp        ax, 0x10c5
00495fef  ja         0x496137

// block 00495ff5
00495ff5  movlpd     xmm1, qword ptr [0x4a86f0]
00495ffd  mulsd      xmm1, xmm0
00496001  movlpd     xmm2, qword ptr [0x4a86f8]
00496009  cvtsd2si   edx, xmm1
0049600d  addsd      xmm1, xmm2
00496011  movlpd     xmm3, qword ptr [0x4a8710]
00496019  subsd      xmm1, xmm2
0049601d  movapd     xmm2, xmmword ptr [0x4a8700]
00496025  mulsd      xmm3, xmm1
00496029  unpcklpd   xmm1, xmm1
0049602d  add        edx, 0x1c7610
00496033  movsd      xmm4, xmm0
00496037  and        edx, 0x3f
0049603a  movapd     xmm5, xmmword ptr [0x4a86e0]
00496042  lea        eax, [0x4a7eb0]
00496048  shl        edx, 5
0049604b  add        eax, edx
0049604d  mulpd      xmm2, xmm1
00496051  subsd      xmm0, xmm3
00496055  mulsd      xmm1, qword ptr [0x4a8718]
0049605d  subsd      xmm4, xmm3
00496061  movlpd     xmm7, qword ptr [eax + 8]
00496066  unpcklpd   xmm0, xmm0
0049606a  movsd      xmm3, xmm4
0049606e  subsd      xmm4, xmm2
00496072  mulpd      xmm5, xmm0
00496076  subpd      xmm0, xmm2
0049607a  movapd     xmm6, xmmword ptr [0x4a86c0]
00496082  mulsd      xmm7, xmm4
00496086  subsd      xmm3, xmm4
0049608a  mulpd      xmm5, xmm0
0049608e  mulpd      xmm0, xmm0
00496092  subsd      xmm3, xmm2
00496096  movapd     xmm2, xmmword ptr [eax]
0049609a  subsd      xmm1, xmm3
0049609e  movlpd     xmm3, qword ptr [eax + 0x18]
004960a3  addsd      xmm2, xmm3
004960a7  subsd      xmm7, xmm2
004960ab  mulsd      xmm2, xmm4
004960af  mulpd      xmm6, xmm0
004960b3  mulsd      xmm3, xmm4
004960b7  mulpd      xmm2, xmm0
004960bb  mulpd      xmm0, xmm0
004960bf  addpd      xmm5, xmmword ptr [0x4a86d0]
004960c7  mulsd      xmm4, qword ptr [eax]
004960cb  addpd      xmm6, xmmword ptr [0x4a86b0]
004960d3  mulpd      xmm5, xmm0
004960d7  movsd      xmm0, xmm3
004960db  addsd      xmm3, qword ptr [eax + 8]
004960e0  mulsd      xmm1, xmm7
004960e4  movsd      xmm7, xmm4
004960e8  addsd      xmm4, xmm3
004960ec  addpd      xmm6, xmm5
004960f0  movlpd     xmm5, qword ptr [eax + 8]
004960f5  subsd      xmm5, xmm3
004960f9  subsd      xmm3, xmm4
004960fd  addsd      xmm1, qword ptr [eax + 0x10]
00496102  mulpd      xmm6, xmm2
00496106  addsd      xmm5, xmm0
0049610a  addsd      xmm3, xmm7
0049610e  addsd      xmm1, xmm5
00496112  addsd      xmm1, xmm3
00496116  addsd      xmm1, xmm6
0049611a  unpckhpd   xmm6, xmm6
0049611e  addsd      xmm1, xmm6
00496122  sub        esp, 0x10
00496125  addsd      xmm4, xmm1
00496129  movlpd     qword ptr [esp + 4], xmm4
0049612f  fld        qword ptr [esp + 4]
00496133  add        esp, 0x10
00496136  ret        

// block 00496137
00496137  jg         0x496164

// block 00496139
00496139  pextrw     eax, xmm0, 3
0049613e  and        ax, 0x7fff
00496142  pinsrw     xmm0, eax, 3
00496147  sub        esp, 0x10
0049614a  movlpd     xmm1, qword ptr [0x4a8740]
00496152  subsd      xmm1, xmm0
00496156  movlpd     qword ptr [esp + 4], xmm1
0049615c  fld        qword ptr [esp + 4]
00496160  add        esp, 0x10
00496163  ret        

// block 00496164
00496164  jmp        0x493a3f
