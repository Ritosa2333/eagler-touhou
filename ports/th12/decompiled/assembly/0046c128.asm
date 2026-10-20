
// block 0046c128
0046c128  or         al, 0xff
0046c12a  inc        dword ptr [eax]
0046c130  add        byte ptr [eax], al
0046c132  add        byte ptr [eax], al
0046c134  jo         0x46c136

// block 0046c136
0046c136  add        byte ptr [eax], al
0046c138  or         al, 0xff
0046c13a  inc        dword ptr [eax]
0046c140  add        byte ptr [eax], al
0046c142  add        byte ptr [eax], al
0046c144  jno        0x46c146

// block 0046c146
0046c146  add        byte ptr [eax], al
0046c148  or         al, 0xff
0046c14a  inc        dword ptr [eax]
0046c150  add        byte ptr [eax], al
0046c152  add        byte ptr [eax], al
0046c154  jb         0x46c156

// block 0046c156
0046c156  add        byte ptr [eax], al
0046c158  or         al, 0xff
0046c15a  inc        dword ptr [eax]
0046c160  add        byte ptr [eax], al
0046c162  add        byte ptr [eax], al
0046c164  jae        0x46c166

// block 0046c166
0046c166  add        byte ptr [eax], al
0046c168  or         al, 0xff
0046c16a  inc        dword ptr [eax]
0046c170  add        byte ptr [eax], al
0046c172  add        byte ptr [eax], al
0046c174  je         0x46c176

// block 0046c176
0046c176  add        byte ptr [eax], al
0046c178  or         al, 0xff
0046c17a  inc        dword ptr [eax]
0046c180  add        byte ptr [eax], al
0046c182  add        byte ptr [eax], al
0046c184  jne        0x46c186

// block 0046c186
0046c186  add        byte ptr [eax], al
0046c188  or         al, 0xff
0046c18a  inc        dword ptr [eax]
0046c190  add        byte ptr [eax], al
0046c192  add        byte ptr [eax], al
0046c194  jbe        0x46c196

// block 0046c196
0046c196  add        byte ptr [eax], al
0046c198  or         al, 0xff
0046c19a  inc        dword ptr [eax]
0046c1a0  add        byte ptr [eax], al
0046c1a2  add        byte ptr [eax], al
0046c1a4  ja         0x46c1a6

// block 0046c1a6
0046c1a6  add        byte ptr [eax], al
0046c1a8  or         al, 0xff
0046c1aa  inc        dword ptr [eax]
0046c1b0  add        byte ptr [eax], al
0046c1b2  add        byte ptr [eax], al
0046c1b4  js         0x46c1b6

// block 0046c1b6
0046c1b6  add        byte ptr [eax], al
0046c1b8  or         al, 0xff
0046c1ba  inc        dword ptr [eax]
0046c1c0  add        byte ptr [eax], al
0046c1c2  add        byte ptr [eax], al
0046c1c4  jns        0x46c1c6

// block 0046c1c6
0046c1c6  add        byte ptr [eax], al
0046c1c8  or         al, 0xff
0046c1ca  inc        dword ptr [eax]
0046c1d0  add        byte ptr [eax], al
0046c1d2  add        byte ptr [eax], al
0046c1d4  jp         0x46c1d6

// block 0046c1d6
0046c1d6  add        byte ptr [eax], al
0046c1d8  or         al, 0xff
0046c1da  inc        dword ptr [eax]
0046c1e0  add        byte ptr [eax], al
0046c1e2  add        byte ptr [eax], al
0046c1e4  jnp        0x46c1e6

// block 0046c1e6
0046c1e6  add        byte ptr [eax], al
0046c1e8  or         al, 0xff
0046c1ea  inc        dword ptr [eax]
0046c1f0  add        byte ptr [eax], al
0046c1f2  add        byte ptr [eax], al
0046c1f4  jl         0x46c1f6

// block 0046c1f6
0046c1f6  add        byte ptr [eax], al
0046c1f8  or         al, 0xff
0046c1fa  inc        dword ptr [eax]
0046c200  add        byte ptr [eax], al
0046c202  add        byte ptr [eax], al
0046c204  jge        0x46c206

// block 0046c206
0046c206  add        byte ptr [eax], al
0046c208  or         al, 0xff
0046c20a  inc        dword ptr [eax]
0046c210  add        byte ptr [eax], al
0046c212  add        byte ptr [eax], al
0046c214  jle        0x46c216

// block 0046c216
0046c216  add        byte ptr [eax], al
0046c218  or         al, 0xff
0046c21a  inc        dword ptr [eax]
0046c220  add        byte ptr [eax], al
0046c222  add        byte ptr [eax], al
0046c224  jg         0x46c226

// block 0046c226
0046c226  add        byte ptr [eax], al
0046c228  or         al, 0xff
0046c22a  inc        dword ptr [eax]
0046c230  add        byte ptr [eax], al
0046c232  add        byte ptr [eax], al
0046c234  add        byte ptr [eax], 0
0046c237  add        byte ptr [edi + edi*8], cl
0046c23a  inc        dword ptr [eax]
0046c240  add        byte ptr [eax], al
0046c242  add        byte ptr [eax], al
0046c244  add        dword ptr [eax], 0xff0c0000
0046c24a  inc        dword ptr [eax]
0046c250  add        byte ptr [eax], al
0046c252  add        byte ptr [eax], al
0046c254  add        byte ptr [eax], 0
0046c257  add        byte ptr [edi + edi*8], cl
0046c25a  inc        dword ptr [eax]
0046c260  add        byte ptr [eax], al
0046c262  add        byte ptr [eax], al
0046c264  add        dword ptr [eax], 0
0046c267  add        byte ptr [edi + edi*8], cl
0046c26a  inc        dword ptr [eax]
0046c270  add        byte ptr [eax], al
0046c272  add        byte ptr [eax], al
0046c274  test       byte ptr [eax], al
0046c276  add        byte ptr [eax], al
0046c278  or         al, 0xff
0046c27a  inc        dword ptr [eax]
0046c280  add        byte ptr [eax], al
0046c282  add        byte ptr [eax], al
0046c284  test       dword ptr [eax], eax
0046c286  add        byte ptr [eax], al
0046c288  or         al, 0xff
0046c28a  inc        dword ptr [eax]
0046c290  add        byte ptr [eax], al
0046c292  add        byte ptr [eax], al
0046c294  xchg       byte ptr [eax], al
0046c296  add        byte ptr [eax], al
0046c298  or         al, 0xff
0046c29a  inc        dword ptr [eax]
0046c2a0  add        byte ptr [eax], al
0046c2a2  add        byte ptr [eax], al
0046c2a4  xchg       dword ptr [eax], eax
0046c2a6  add        byte ptr [eax], al
0046c2a8  or         al, 0xff
0046c2aa  inc        dword ptr [eax]
0046c2b0  add        byte ptr [eax], al
0046c2b2  add        byte ptr [eax], al
0046c2b4  mov        byte ptr [eax], al
0046c2b6  add        byte ptr [eax], al
0046c2b8  or         al, 0xff
0046c2ba  inc        dword ptr [eax]
0046c2c0  add        byte ptr [eax], al
0046c2c2  add        byte ptr [eax], al
0046c2c4  mov        dword ptr [eax], eax
0046c2c6  add        byte ptr [eax], al
0046c2c8  or         al, 0xff
0046c2ca  inc        dword ptr [eax]
0046c2d0  add        byte ptr [eax], al
0046c2d2  add        byte ptr [eax], al
0046c2d4  mov        al, byte ptr [eax]
0046c2d6  add        byte ptr [eax], al
0046c2d8  or         al, 0xff
0046c2da  inc        dword ptr [eax]
0046c2e0  add        byte ptr [eax], al
0046c2e2  add        byte ptr [eax], al
0046c2e4  mov        eax, dword ptr [eax]
0046c2e6  add        byte ptr [eax], al
0046c2e8  or         al, 0xff
0046c2ea  inc        dword ptr [eax]
0046c2f0  add        byte ptr [eax], al
0046c2f2  add        byte ptr [eax], al
0046c2f4  mov        word ptr [eax], es
0046c2f6  add        byte ptr [eax], al
0046c2f8  or         al, 0xff
0046c2fa  inc        dword ptr [eax]
0046c300  add        byte ptr [eax], al
0046c302  add        byte ptr [eax], al
0046c304  lea        eax, [eax]
0046c306  add        byte ptr [eax], al
0046c308  or         al, 0xff
0046c30a  inc        dword ptr [eax]
0046c310  add        byte ptr [eax], al
0046c312  add        byte ptr [eax], al
0046c314  mov        es, word ptr [eax]
0046c316  add        byte ptr [eax], al
0046c318  or         al, 0xff
0046c31a  inc        dword ptr [eax]
0046c320  add        byte ptr [eax], al
0046c322  add        byte ptr [eax], al
0046c324  pop        dword ptr [eax]
0046c326  add        byte ptr [eax], al
0046c328  or         al, 0xff
0046c32a  inc        dword ptr [eax]
0046c330  add        byte ptr [eax], al
0046c332  add        byte ptr [eax], al
0046c334  nop        
0046c335  add        byte ptr [eax], al
0046c337  add        byte ptr [edi + edi*8], cl

// block 0046c33a
0046c33a  inc        dword ptr [eax]
0046c340  add        byte ptr [eax], al
0046c342  add        byte ptr [eax], al
0046c344  xchg       ecx, eax
0046c345  add        byte ptr [eax], al
0046c347  add        byte ptr [edi + edi*8], cl
0046c34a  inc        dword ptr [eax]
0046c350  add        byte ptr [eax], al
0046c352  add        byte ptr [eax], al
0046c354  xchg       edx, eax
0046c355  add        byte ptr [eax], al
0046c357  add        byte ptr [edi + edi*8], cl
0046c35a  inc        dword ptr [eax]
0046c360  add        byte ptr [eax], al
0046c362  add        byte ptr [eax], al
0046c364  xchg       ebx, eax
0046c365  add        byte ptr [eax], al
0046c367  add        byte ptr [edi + edi*8], cl
0046c36a  inc        dword ptr [eax]
0046c370  add        byte ptr [eax], al
0046c372  add        byte ptr [eax], al
0046c374  xchg       esp, eax
0046c375  add        byte ptr [eax], al
0046c377  add        byte ptr [edi + edi*8], cl
0046c37a  inc        dword ptr [eax]
0046c380  add        byte ptr [eax], al
0046c382  add        byte ptr [eax], al
0046c384  xchg       ebp, eax
0046c385  add        byte ptr [eax], al
0046c387  add        byte ptr [edi + edi*8], cl
0046c38a  inc        dword ptr [eax]
0046c390  add        byte ptr [eax], al
0046c392  add        byte ptr [eax], al
0046c394  xchg       esi, eax
0046c395  add        byte ptr [eax], al
0046c397  add        byte ptr [edi + edi*8], cl
0046c39a  inc        dword ptr [eax]
0046c3a0  add        byte ptr [eax], al
0046c3a2  add        byte ptr [eax], al
0046c3a4  xchg       edi, eax
0046c3a5  add        byte ptr [eax], al
0046c3a7  add        byte ptr [edi + edi*8], cl
0046c3aa  inc        dword ptr [eax]
0046c3b0  add        byte ptr [eax], al
0046c3b2  add        byte ptr [eax], al
0046c3b4  cwde       
0046c3b5  add        byte ptr [eax], al
0046c3b7  add        byte ptr [edi + edi*8], cl
0046c3ba  inc        dword ptr [eax]
0046c3c0  add        byte ptr [eax], al
0046c3c2  add        byte ptr [eax], al
0046c3c4  cdq        
0046c3c5  add        byte ptr [eax], al
0046c3c7  add        byte ptr [edi + edi*8], cl
0046c3ca  inc        dword ptr [eax]
0046c3d0  add        byte ptr [eax], al
0046c3d2  add        byte ptr [eax], al
