import {registerEaglerHostTests} from '../../../tools/ports/eagler-host-test-suite.mjs';
await registerEaglerHostTests({owner:'th08',testUrl:import.meta.url,
  fontAliases:[{language:null,source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'}]});
