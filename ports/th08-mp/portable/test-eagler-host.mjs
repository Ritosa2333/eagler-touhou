import {registerEaglerHostTests} from '../../../tools/ports/eagler-host-test-suite.mjs';
await registerEaglerHostTests({owner:'th08-mp',testUrl:import.meta.url,
  fontAliases:[{language:null,source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'}]});
