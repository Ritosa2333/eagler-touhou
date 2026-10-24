import {PresentationLabControllerCore} from '../../../../common/testkit/presentation-lab/controller-core.mjs';
import {Th08RuntimeDriver,Th08ObservationAdapter} from './adapter.mjs';

export class LabController extends PresentationLabControllerCore{
  constructor(runtime,identity){
    const driver=new Th08RuntimeDriver(runtime,identity),observer=new Th08ObservationAdapter(runtime);
    super({driver,observer,identity});
    if(this.capabilities.features.productionFrameCallbackCadence===true&&typeof driver.frameAt!=='function')
      throw Error('Production frame callback capability is missing driver.frameAt()');
    // Transitional title helpers still use these fields. Common orchestration
    // itself never reaches through them.
    this.runtime=runtime;
    this.core=runtime.core;
  }
  frameAt(nowMs){
    if(this.capabilities.features.productionFrameCallbackCadence!==true)
      throw Error('Production frame callback cadence is not available for this Runtime');
    return this.driver.frameAt(nowMs);
  }
  setPresentationLimit(enabled){
    if(typeof enabled!=='boolean')throw new TypeError('Presentation limit must be boolean');
    const options=this.runtime.Module?.eaglerOptions;
    if(!options||typeof options!=='object')throw Error('Runtime presentation options are unavailable');
    options.limitPresentationTo60=enabled;
  }
}
