import {testOutsideTouchContract} from '../../../tools/ports/outside-touch-contract.mjs';
import {bindOutsideTouches} from '../web/sdl-runtime/eagler-host.mjs';

testOutsideTouchContract(bindOutsideTouches);
