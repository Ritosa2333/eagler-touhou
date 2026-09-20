import {PresentationLabControllerCore} from '../../third_party/eagler-common/testkit/presentation-lab/controller-core.mjs';
import {Th08RuntimeDriver,Th08ObservationAdapter} from './adapter.mjs';

export class LabController extends PresentationLabControllerCore{
  constructor(runtime,identity){
    const driver=new Th08RuntimeDriver(runtime,identity),observer=new Th08ObservationAdapter(runtime);
    super({driver,observer,identity});
    // Transitional title helpers still use these fields. Common orchestration
    // itself never reaches through them.
    this.runtime=runtime;
    this.core=runtime.core;
  }
}
