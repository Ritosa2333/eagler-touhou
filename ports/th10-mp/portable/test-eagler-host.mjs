import {registerEaglerHostTests} from '../../../tools/ports/eagler-host-test-suite.mjs';
await registerEaglerHostTests({owner:'th10-mp',testUrl:import.meta.url,
  fontAliases:[
    {language:'jp',source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'},
    {language:'chs',source:'/unifont.otf',target:'/fonts/simhei.ttf'},
  ]});
