import {componentFileIds} from '../../package/package-generation.mjs';
import type {ProductId} from '../../src/contracts/product-catalog.mts';
import type {PackageDescriptor} from '../../src/contracts/package-read-models.mts';

export function includesOggTrack(productId: ProductId | undefined, path: string) {
  // The shared TH05 package also serves the single-player Music Room.
  return productId !== 'th05mp' || !/(?:^|\/)room-[^/]+\.ogg$/i.test(path);
}

export function runtimeOggFileIds(descriptor: PackageDescriptor, productId?: ProductId) {
  return componentFileIds(descriptor, 'ogg').filter(id => includesOggTrack(productId, descriptor.files[id].target));
}
