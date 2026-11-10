import {PresentationLabControllerCore} from '../../third_party/eagler-common/testkit/presentation-lab/controller-core.mjs';
import {Th11RuntimeDriver,Th11ObservationAdapter} from './adapter.mjs';
export class LabController extends PresentationLabControllerCore{
  constructor(runtime,identity){
    const driver=new Th11RuntimeDriver(runtime,identity),observer=new Th11ObservationAdapter(runtime);
    super({driver,observer,identity});
    this.runtime=runtime;
    this.core=runtime.core;
  }
}
