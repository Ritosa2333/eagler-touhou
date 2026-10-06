
// block 0046b61a
0046b61a  add        byte ptr [eax], al
0046b620  cld        
0046b621  xchg       esp, eax
0046b622  dec        ecx
0046b623  add        byte ptr [ebx + 0xc000000], bl
0046b629  wait       
0046b62a  add        byte ptr [eax], al
0046b630  cld        
0046b631  xchg       esp, eax
0046b632  dec        ecx
0046b633  add        byte ptr [eax + eax - 0x63f40000], bl
0046b63a  add        byte ptr [eax], al
0046b640  cld        
0046b641  xchg       esp, eax
0046b642  dec        ecx
0046b643  add        byte ptr [ebp + 0xc000000], bl
0046b649  popfd      
0046b64a  add        byte ptr [eax], al
0046b650  cld        
0046b651  xchg       esp, eax
0046b652  dec        ecx
0046b653  add        byte ptr [esi + 0xc000000], bl
0046b659  sahf       
0046b65a  add        byte ptr [eax], al
0046b660  cld        
0046b661  xchg       esp, eax
0046b662  dec        ecx
0046b663  add        byte ptr [edi + 0xc000000], bl
0046b669  lahf       
0046b66a  add        byte ptr [eax], al
0046b670  cld        
0046b671  xchg       esp, eax
0046b672  dec        ecx
0046b673  add        byte ptr [eax + 0xc000000], ah
0046b679  mov        al, byte ptr [0x8000]
0046b67e  add        byte ptr [eax], al
0046b680  cld        
0046b681  xchg       esp, eax
0046b682  dec        ecx
0046b683  add        byte ptr [ecx + 0xc000000], ah
0046b689  mov        eax, dword ptr [0x8000]
0046b68e  add        byte ptr [eax], al
0046b690  cld        
0046b691  xchg       esp, eax
0046b692  dec        ecx
0046b693  add        byte ptr [edx + 0xc000000], ah
0046b699  mov        byte ptr [0x8000], al
0046b69e  add        byte ptr [eax], al
0046b6a0  cld        
0046b6a1  xchg       esp, eax
0046b6a2  dec        ecx
0046b6a3  add        byte ptr [ebx + 0xc000000], ah
0046b6a9  mov        dword ptr [0x8000], eax
0046b6ae  add        byte ptr [eax], al
0046b6b0  cld        
0046b6b1  xchg       esp, eax
0046b6b2  dec        ecx
0046b6b3  add        byte ptr [eax + eax - 0x5bf40000], ah
0046b6ba  add        byte ptr [eax], al
0046b6c0  cld        
0046b6c1  xchg       esp, eax
0046b6c2  dec        ecx
0046b6c3  add        byte ptr [ebp + 0xc000000], ah
0046b6c9  movsd      dword ptr es:[edi], dword ptr [esi]
0046b6ca  add        byte ptr [eax], al
0046b6d0  cld        
0046b6d1  xchg       esp, eax
0046b6d2  dec        ecx
0046b6d3  add        byte ptr [esi + 0xc000000], ah
0046b6d9  cmpsb      byte ptr [esi], byte ptr es:[edi]
0046b6da  add        byte ptr [eax], al
0046b6e0  cld        
0046b6e1  xchg       esp, eax
0046b6e2  dec        ecx
0046b6e3  add        byte ptr [edi + 0xc000000], ah
0046b6e9  cmpsd      dword ptr [esi], dword ptr es:[edi]
0046b6ea  add        byte ptr [eax], al
0046b6f0  cld        
0046b6f1  xchg       esp, eax
0046b6f2  dec        ecx
0046b6f3  add        byte ptr [eax + 0xc000000], ch
0046b6f9  test       al, 0
0046b6fb  add        byte ptr [eax], 0
0046b6fe  add        byte ptr [eax], al
0046b700  cld        
0046b701  xchg       esp, eax
0046b702  dec        ecx
0046b703  add        byte ptr [ecx + 0xc000000], ch
0046b709  test       eax, 0x8000
0046b70e  add        byte ptr [eax], al
0046b710  cld        
0046b711  xchg       esp, eax
0046b712  dec        ecx
0046b713  add        byte ptr [edx + 0xc000000], ch
0046b719  stosb      byte ptr es:[edi], al
0046b71a  add        byte ptr [eax], al
0046b720  cld        
0046b721  xchg       esp, eax
0046b722  dec        ecx

// block 0046b723
0046b723  add        byte ptr [ebx + 0xc000000], ch
0046b729  stosd      dword ptr es:[edi], eax
0046b72a  add        byte ptr [eax], al
0046b730  cld        
0046b731  xchg       esp, eax
0046b732  dec        ecx
0046b733  add        byte ptr [eax + eax - 0x53f40000], ch
0046b73a  add        byte ptr [eax], al
0046b740  cld        
0046b741  xchg       esp, eax
0046b742  dec        ecx
0046b743  add        byte ptr [ebp + 0xc000000], ch
0046b749  lodsd      eax, dword ptr [esi]
0046b74a  add        byte ptr [eax], al
0046b750  cld        
0046b751  xchg       esp, eax
0046b752  dec        ecx
0046b753  add        byte ptr [esi + 0xc000000], ch
0046b759  scasb      al, byte ptr es:[edi]
0046b75a  add        byte ptr [eax], al
0046b760  cld        
0046b761  xchg       esp, eax
0046b762  dec        ecx
0046b763  add        byte ptr [edi + 0xc000000], ch
0046b769  scasd      eax, dword ptr es:[edi]
0046b76a  add        byte ptr [eax], al
0046b770  cld        
0046b771  xchg       esp, eax
0046b772  dec        ecx
0046b773  add        byte ptr [eax + 0xc000000], dh
0046b779  mov        al, 0
0046b77b  add        byte ptr [eax], 0
0046b77e  add        byte ptr [eax], al
0046b780  cld        
0046b781  xchg       esp, eax
0046b782  dec        ecx
0046b783  add        byte ptr [ecx + 0xc000000], dh
0046b789  mov        cl, 0
0046b78b  add        byte ptr [eax], 0
0046b78e  add        byte ptr [eax], al
0046b790  cld        
0046b791  xchg       esp, eax
0046b792  dec        ecx
0046b793  add        byte ptr [edx + 0xc000000], dh
0046b799  mov        dl, 0
0046b79b  add        byte ptr [eax], 0
0046b79e  add        byte ptr [eax], al
0046b7a0  cld        
0046b7a1  xchg       esp, eax
0046b7a2  dec        ecx
0046b7a3  add        byte ptr [ebx + 0xc000000], dh
0046b7a9  mov        bl, 0
0046b7ab  add        byte ptr [eax], 0
0046b7ae  add        byte ptr [eax], al
0046b7b0  cld        
0046b7b1  xchg       esp, eax
0046b7b2  dec        ecx
0046b7b3  add        byte ptr [eax + eax - 0x4bf40000], dh
0046b7ba  add        byte ptr [eax], al
0046b7c0  cld        
0046b7c1  xchg       esp, eax
0046b7c2  dec        ecx
0046b7c3  add        byte ptr [ebp + 0xc000000], dh
0046b7c9  mov        ch, 0
0046b7cb  add        byte ptr [eax], 0
0046b7ce  add        byte ptr [eax], al
0046b7d0  cld        
0046b7d1  xchg       esp, eax
0046b7d2  dec        ecx
0046b7d3  add        byte ptr [esi + 0xc000000], dh
0046b7d9  mov        dh, 0
0046b7db  add        byte ptr [eax], 0
0046b7de  add        byte ptr [eax], al
0046b7e0  cld        
0046b7e1  xchg       esp, eax
0046b7e2  dec        ecx
0046b7e3  add        byte ptr [edi + 0xc000000], dh
0046b7e9  mov        bh, 0
0046b7eb  add        byte ptr [eax], 0
0046b7ee  add        byte ptr [eax], al
0046b7f0  cld        
0046b7f1  xchg       esp, eax
0046b7f2  dec        ecx
0046b7f3  add        byte ptr [eax + 0xc000000], bh
0046b7f9  mov        eax, 0x8000
0046b7fe  add        byte ptr [eax], al
0046b800  cld        
0046b801  xchg       esp, eax
0046b802  dec        ecx
0046b803  add        byte ptr [ecx + 0xc000000], bh
0046b809  mov        ecx, 0x8000
0046b80e  add        byte ptr [eax], al
0046b810  cld        
0046b811  xchg       esp, eax
0046b812  dec        ecx
0046b813  add        byte ptr [edx + 0xc000000], bh
0046b819  mov        edx, 0x8000
0046b81e  add        byte ptr [eax], al
0046b820  cld        

// block 0046b821
0046b821  xchg       esp, eax
0046b822  dec        ecx
0046b823  add        byte ptr [ebx + 0xc000000], bh
0046b829  mov        ebx, 0x8000
0046b82e  add        byte ptr [eax], al
0046b830  cld        
0046b831  xchg       esp, eax
0046b832  dec        ecx
0046b833  add        byte ptr [eax + eax - 0x43f40000], bh
0046b83a  add        byte ptr [eax], al
0046b840  cld        
0046b841  xchg       esp, eax
0046b842  dec        ecx
0046b843  add        byte ptr [ebp + 0xc000000], bh
0046b849  mov        ebp, 0x8000
0046b84e  add        byte ptr [eax], al
0046b850  cld        
0046b851  xchg       esp, eax
0046b852  dec        ecx
0046b853  add        byte ptr [esi + 0xc000000], bh
0046b859  mov        esi, 0x8000
0046b85e  add        byte ptr [eax], al
0046b860  cld        
0046b861  xchg       esp, eax
0046b862  dec        ecx
0046b863  add        byte ptr [edi + 0xc000000], bh
0046b869  mov        edi, 0x8000
0046b86e  add        byte ptr [eax], al
0046b870  cld        
0046b871  xchg       esp, eax
0046b872  dec        ecx
0046b873  add        al, al
0046b875  add        byte ptr [eax], al
0046b877  add        byte ptr [eax + eax*8], cl
0046b87a  add        byte ptr [eax], al
0046b880  cld        
0046b881  xchg       esp, eax
0046b882  dec        ecx
0046b883  add        cl, al
0046b885  add        byte ptr [eax], al
0046b887  add        byte ptr [ecx + eax*8], cl
0046b88a  add        byte ptr [eax], al
0046b890  cld        
0046b891  xchg       esp, eax
0046b892  dec        ecx
0046b893  add        dl, al
0046b895  add        byte ptr [eax], al
0046b897  add        byte ptr [edx + eax*8], cl
0046b89a  add        byte ptr [eax], al
0046b8a0  cld        
0046b8a1  xchg       esp, eax
0046b8a2  dec        ecx
0046b8a3  add        bl, al
0046b8a5  add        byte ptr [eax], al
0046b8a7  add        byte ptr [ebx + eax*8], cl
0046b8aa  add        byte ptr [eax], al
0046b8b0  cld        
0046b8b1  xchg       esp, eax
0046b8b2  dec        ecx
0046b8b3  add        ah, al
0046b8b5  add        byte ptr [eax], al
0046b8b7  add        byte ptr [esp + eax*8], cl
0046b8ba  add        byte ptr [eax], al
0046b8c0  cld        
0046b8c1  xchg       esp, eax
0046b8c2  dec        ecx
0046b8c3  add        ch, al
0046b8c5  add        byte ptr [eax], al
0046b8c7  add        byte ptr [eax*8 + 0x8000], cl
0046b8ce  add        byte ptr [eax], al
0046b8d0  cld        
0046b8d1  xchg       esp, eax
0046b8d2  dec        ecx
0046b8d3  add        dh, al
0046b8d5  add        byte ptr [eax], al
0046b8d7  add        byte ptr [esi + eax*8], cl
0046b8da  add        byte ptr [eax], al
0046b8e0  cld        
0046b8e1  xchg       esp, eax
0046b8e2  dec        ecx
0046b8e3  add        bh, al
0046b8e5  add        byte ptr [eax], al
0046b8e7  add        byte ptr [edi + eax*8], cl
0046b8ea  add        byte ptr [eax], al
0046b8f0  cld        
0046b8f1  xchg       esp, eax
0046b8f2  dec        ecx
0046b8f3  add        al, cl
0046b8f5  add        byte ptr [eax], al
0046b8f7  add        byte ptr [eax + ecx*8], cl
0046b8fa  add        byte ptr [eax], al
0046b900  cld        
0046b901  xchg       esp, eax
0046b902  dec        ecx
0046b903  add        cl, cl
0046b905  add        byte ptr [eax], al
0046b907  add        byte ptr [ecx + ecx*8], cl
0046b90a  add        byte ptr [eax], al
0046b910  cld        

// block 0046b911
0046b911  xchg       esp, eax
0046b912  dec        ecx
0046b913  add        dl, cl
0046b915  add        byte ptr [eax], al
0046b917  add        byte ptr [edx + ecx*8], cl
0046b91a  add        byte ptr [eax], al
0046b920  cld        
0046b921  xchg       esp, eax
0046b922  dec        ecx
0046b923  add        bl, cl
0046b925  add        byte ptr [eax], al
0046b927  add        byte ptr [ebx + ecx*8], cl
0046b92a  add        byte ptr [eax], al
0046b930  cld        
0046b931  xchg       esp, eax
0046b932  dec        ecx
0046b933  add        ah, cl
0046b935  add        byte ptr [eax], al
0046b937  add        byte ptr [esp + ecx*8], cl
0046b93a  add        byte ptr [eax], al
0046b940  cld        
0046b941  xchg       esp, eax
0046b942  dec        ecx
0046b943  add        ch, cl
0046b945  add        byte ptr [eax], al
0046b947  add        byte ptr [ecx*8 + 0x8000], cl
0046b94e  add        byte ptr [eax], al
0046b950  cld        
0046b951  xchg       esp, eax
0046b952  dec        ecx
0046b953  add        dh, cl
0046b955  add        byte ptr [eax], al
0046b957  add        byte ptr [esi + ecx*8], cl
0046b95a  add        byte ptr [eax], al
0046b960  cld        
0046b961  xchg       esp, eax
0046b962  dec        ecx
0046b963  add        bh, cl
0046b965  add        byte ptr [eax], al
0046b967  add        byte ptr [edi + ecx*8], cl
0046b96a  add        byte ptr [eax], al
0046b970  cld        
0046b971  xchg       esp, eax
0046b972  dec        ecx
0046b973  add        al, dl
0046b975  add        byte ptr [eax], al
0046b977  add        byte ptr [eax + edx*8], cl
0046b97a  add        byte ptr [eax], al
0046b980  cld        
0046b981  xchg       esp, eax
0046b982  dec        ecx
0046b983  add        cl, dl
0046b985  add        byte ptr [eax], al
0046b987  add        byte ptr [ecx + edx*8], cl
0046b98a  add        byte ptr [eax], al
0046b990  cld        
0046b991  xchg       esp, eax
0046b992  dec        ecx
0046b993  add        dl, dl
0046b995  add        byte ptr [eax], al
0046b997  add        byte ptr [edx + edx*8], cl
0046b99a  add        byte ptr [eax], al
0046b9a0  cld        
0046b9a1  xchg       esp, eax
0046b9a2  dec        ecx
0046b9a3  add        bl, dl
0046b9a5  add        byte ptr [eax], al
0046b9a7  add        byte ptr [ebx + edx*8], cl
0046b9aa  add        byte ptr [eax], al
0046b9b0  cld        
0046b9b1  xchg       esp, eax
0046b9b2  dec        ecx
0046b9b3  add        ah, dl
0046b9b5  add        byte ptr [eax], al
0046b9b7  add        byte ptr [esp + edx*8], cl
0046b9ba  add        byte ptr [eax], al
0046b9c0  cld        
0046b9c1  xchg       esp, eax
0046b9c2  dec        ecx
0046b9c3  add        ch, dl
0046b9c5  add        byte ptr [eax], al
0046b9c7  add        byte ptr [edx*8 + 0x8000], cl
0046b9ce  add        byte ptr [eax], al
0046b9d0  cld        
0046b9d1  xchg       esp, eax
0046b9d2  dec        ecx
0046b9d3  add        dh, dl
0046b9d5  add        byte ptr [eax], al
0046b9d7  add        byte ptr [esi + edx*8], cl
0046b9da  add        byte ptr [eax], al
0046b9e0  cld        
0046b9e1  xchg       esp, eax
0046b9e2  dec        ecx
0046b9e3  add        bh, dl
0046b9e5  add        byte ptr [eax], al
0046b9e7  add        byte ptr [edi + edx*8], cl
0046b9ea  add        byte ptr [eax], al
0046b9f0  cld        
0046b9f1  xchg       esp, eax

// block 0046b9f2
0046b9f2  dec        ecx
0046b9f3  add        al, bl
0046b9f5  add        byte ptr [eax], al
0046b9f7  add        byte ptr [eax + ebx*8], cl
0046b9fa  add        byte ptr [eax], al
0046ba00  cld        
0046ba01  xchg       esp, eax
0046ba02  dec        ecx
0046ba03  add        cl, bl
0046ba05  add        byte ptr [eax], al
0046ba07  add        byte ptr [ecx + ebx*8], cl
0046ba0a  add        byte ptr [eax], al
0046ba10  cld        
0046ba11  xchg       esp, eax
0046ba12  dec        ecx
0046ba13  add        dl, bl
0046ba15  add        byte ptr [eax], al
0046ba17  add        byte ptr [edx + ebx*8], cl
0046ba1a  add        byte ptr [eax], al
0046ba20  cld        
0046ba21  xchg       esp, eax
0046ba22  dec        ecx
0046ba23  add        bl, bl
0046ba25  add        byte ptr [eax], al
0046ba27  add        byte ptr [ebx + ebx*8], cl
0046ba2a  add        byte ptr [eax], al
0046ba30  cld        
0046ba31  xchg       esp, eax
0046ba32  dec        ecx
0046ba33  add        ah, bl
0046ba35  add        byte ptr [eax], al
0046ba37  add        byte ptr [esp + ebx*8], cl
0046ba3a  add        byte ptr [eax], al
0046ba40  cld        
0046ba41  xchg       esp, eax
0046ba42  dec        ecx
0046ba43  add        ch, bl
0046ba45  add        byte ptr [eax], al
0046ba47  add        byte ptr [ebx*8 + 0x8000], cl
0046ba4e  add        byte ptr [eax], al
0046ba50  cld        
0046ba51  xchg       esp, eax
0046ba52  dec        ecx
0046ba53  add        dh, bl
0046ba55  add        byte ptr [eax], al
0046ba57  add        byte ptr [esi + ebx*8], cl
0046ba5a  add        byte ptr [eax], al
0046ba60  cld        
0046ba61  xchg       esp, eax
0046ba62  dec        ecx
0046ba63  add        bh, bl
0046ba65  add        byte ptr [eax], al
0046ba67  add        byte ptr [edi + ebx*8], cl
0046ba6a  add        byte ptr [eax], al
0046ba70  cld        
0046ba71  xchg       esp, eax
0046ba72  dec        ecx
0046ba73  add        al, ah
0046ba75  add        byte ptr [eax], al
0046ba77  add        byte ptr [eax], cl
0046ba7a  add        byte ptr [eax], al
0046ba80  cld        
0046ba81  xchg       esp, eax
0046ba82  dec        ecx
0046ba83  add        cl, ah
0046ba85  add        byte ptr [eax], al
0046ba87  add        byte ptr [ecx], cl
0046ba8a  add        byte ptr [eax], al
0046ba90  cld        
0046ba91  xchg       esp, eax
0046ba92  dec        ecx
0046ba93  add        dl, ah
0046ba95  add        byte ptr [eax], al
0046ba97  add        byte ptr [edx], cl
0046ba9a  add        byte ptr [eax], al
0046baa0  cld        
0046baa1  xchg       esp, eax
0046baa2  dec        ecx
0046baa3  add        bl, ah
0046baa5  add        byte ptr [eax], al
0046baa7  add        byte ptr [ebx], cl
0046baaa  add        byte ptr [eax], al
0046bab0  cld        
0046bab1  xchg       esp, eax
0046bab2  dec        ecx
0046bab3  add        ah, ah
0046bab5  add        byte ptr [eax], al
0046bab7  add        byte ptr [esp], cl
0046baba  add        byte ptr [eax], al
0046bac0  cld        
0046bac1  xchg       esp, eax
0046bac2  dec        ecx
0046bac3  add        ch, ah
0046bac5  add        byte ptr [eax], al
0046bac7  add        byte ptr [0x8000], cl
0046bace  add        byte ptr [eax], al
0046bad0  cld        
0046bad1  xchg       esp, eax
0046bad2  dec        ecx

// block 0046bad3
0046bad3  add        dh, ah
0046bad5  add        byte ptr [eax], al
0046bad7  add        byte ptr [esi], cl
0046bada  add        byte ptr [eax], al
0046bae0  cld        
0046bae1  xchg       esp, eax
0046bae2  dec        ecx
0046bae3  add        bh, ah
0046bae5  add        byte ptr [eax], al
0046bae7  add        byte ptr [edi], cl
0046baea  add        byte ptr [eax], al
0046baf0  cld        
0046baf1  xchg       esp, eax
0046baf2  dec        ecx
0046baf3  add        al, ch
0046baf5  add        byte ptr [eax], al
0046baf7  add        byte ptr [eax + ebp*8], cl
0046bafa  add        byte ptr [eax], al
0046bb00  cld        
0046bb01  xchg       esp, eax
0046bb02  dec        ecx
0046bb03  add        cl, ch
0046bb05  add        byte ptr [eax], al
0046bb07  add        byte ptr [ecx + ebp*8], cl
0046bb0a  add        byte ptr [eax], al
0046bb10  cld        
0046bb11  xchg       esp, eax
0046bb12  dec        ecx
0046bb13  add        dl, ch
0046bb15  add        byte ptr [eax], al
0046bb17  add        byte ptr [edx + ebp*8], cl
0046bb1a  add        byte ptr [eax], al
0046bb20  cld        
0046bb21  xchg       esp, eax
0046bb22  dec        ecx
0046bb23  add        bl, ch
0046bb25  add        byte ptr [eax], al
0046bb27  add        byte ptr [ebx + ebp*8], cl
0046bb2a  add        byte ptr [eax], al
0046bb30  cld        
0046bb31  xchg       esp, eax
0046bb32  dec        ecx
0046bb33  add        ah, ch
0046bb35  add        byte ptr [eax], al
0046bb37  add        byte ptr [esp + ebp*8], cl
0046bb3a  add        byte ptr [eax], al
0046bb40  cld        
0046bb41  xchg       esp, eax
0046bb42  dec        ecx
0046bb43  add        ch, ch
0046bb45  add        byte ptr [eax], al
0046bb47  add        byte ptr [ebp*8 + 0x8000], cl
0046bb4e  add        byte ptr [eax], al
0046bb50  cld        
0046bb51  xchg       esp, eax
0046bb52  dec        ecx
0046bb53  add        dh, ch
0046bb55  add        byte ptr [eax], al
0046bb57  add        byte ptr [esi + ebp*8], cl
0046bb5a  add        byte ptr [eax], al
0046bb60  cld        
0046bb61  xchg       esp, eax
0046bb62  dec        ecx
0046bb63  add        bh, ch
0046bb65  add        byte ptr [eax], al
0046bb67  add        byte ptr [edi + ebp*8], cl
0046bb6a  add        byte ptr [eax], al
0046bb70  cld        
0046bb71  xchg       esp, eax
0046bb72  dec        ecx
0046bb73  add        al, dh
0046bb75  add        byte ptr [eax], al
0046bb77  add        byte ptr [eax + esi*8], cl
0046bb7a  add        byte ptr [eax], al
0046bb80  cld        
0046bb81  xchg       esp, eax
0046bb82  dec        ecx
0046bb83  add        cl, dh
0046bb85  add        byte ptr [eax], al
0046bb87  add        byte ptr [ecx + esi*8], cl
0046bb8a  add        byte ptr [eax], al
0046bb90  cld        
0046bb91  xchg       esp, eax
0046bb92  dec        ecx
0046bb93  add        dl, dh
0046bb95  add        byte ptr [eax], al
0046bb97  add        byte ptr [edx + esi*8], cl
0046bb9a  add        byte ptr [eax], al
0046bba0  cld        
0046bba1  xchg       esp, eax
0046bba2  dec        ecx
0046bba3  add        bl, dh
0046bba5  add        byte ptr [eax], al
0046bba7  add        byte ptr [ebx + esi*8], cl
0046bbaa  add        byte ptr [eax], al
0046bbb0  cld        
0046bbb1  xchg       esp, eax
0046bbb2  dec        ecx
0046bbb3  add        ah, dh

// block 0046bbb5
0046bbb5  add        byte ptr [eax], al
0046bbb7  add        byte ptr [esp + esi*8], cl
0046bbba  add        byte ptr [eax], al
0046bbc0  cld        
0046bbc1  xchg       esp, eax
0046bbc2  dec        ecx
0046bbc3  add        ch, dh
0046bbc5  add        byte ptr [eax], al
0046bbc7  add        byte ptr [esi*8 + 0x8000], cl
0046bbce  add        byte ptr [eax], al
0046bbd0  cld        
0046bbd1  xchg       esp, eax
0046bbd2  dec        ecx
0046bbd3  add        dh, dh
0046bbd5  add        byte ptr [eax], al
0046bbd7  add        byte ptr [esi + esi*8], cl
0046bbda  add        byte ptr [eax], al
0046bbe0  cld        
0046bbe1  xchg       esp, eax
0046bbe2  dec        ecx
0046bbe3  add        bh, dh
0046bbe5  add        byte ptr [eax], al
0046bbe7  add        byte ptr [edi + esi*8], cl
0046bbea  add        byte ptr [eax], al
0046bbf0  cld        
0046bbf1  xchg       esp, eax
0046bbf2  dec        ecx
0046bbf3  add        al, bh
0046bbf5  add        byte ptr [eax], al
0046bbf7  add        byte ptr [eax + edi*8], cl
0046bbfa  add        byte ptr [eax], al
0046bc00  cld        
0046bc01  xchg       esp, eax
0046bc02  dec        ecx
0046bc03  add        cl, bh

// block 0046bc06
0046bc06  add        byte ptr [eax], al
0046bc08  or         al, 0xf9
0046bc0a  add        byte ptr [eax], al
0046bc10  cld        
0046bc11  xchg       esp, eax
0046bc12  dec        ecx
0046bc13  add        dl, bh
0046bc15  add        byte ptr [eax], al
0046bc17  add        byte ptr [edx + edi*8], cl
0046bc1a  add        byte ptr [eax], al
0046bc20  cld        
0046bc21  xchg       esp, eax
0046bc22  dec        ecx
0046bc23  add        bl, bh
0046bc25  add        byte ptr [eax], al
0046bc27  add        byte ptr [ebx + edi*8], cl
0046bc2a  add        byte ptr [eax], al
0046bc30  cld        
0046bc31  xchg       esp, eax
0046bc32  dec        ecx
0046bc33  add        ah, bh
0046bc35  add        byte ptr [eax], al
0046bc37  add        byte ptr [esp + edi*8], cl
0046bc3a  add        byte ptr [eax], al
0046bc40  cld        
0046bc41  xchg       esp, eax
0046bc42  dec        ecx
0046bc43  add        ch, bh
0046bc45  add        byte ptr [eax], al
0046bc47  add        byte ptr [edi*8 + 0x8000], cl
0046bc4e  add        byte ptr [eax], al
0046bc50  cld        
0046bc51  xchg       esp, eax
0046bc52  dec        ecx
0046bc53  add        dh, bh
0046bc55  add        byte ptr [eax], al
0046bc57  add        byte ptr [esi + edi*8], cl
0046bc5a  add        byte ptr [eax], al
0046bc60  cld        
0046bc61  xchg       esp, eax
0046bc62  dec        ecx
0046bc63  add        bh, bh
0046bc65  add        byte ptr [eax], al
0046bc67  add        byte ptr [edi + edi*8], cl
0046bc6a  add        byte ptr [eax], al
0046bc70  jl         0x46bc06

// block 0046bc72
0046bc72  dec        ecx
0046bc73  add        byte ptr [eax], al
0046bc75  add        byte ptr [eax], al
0046bc77  add        byte ptr [ebx], al
