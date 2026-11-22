import type {LibraryProduct} from './products';
import th01 from './book-covers/th01.webp?url';
import th02 from './book-covers/th02.webp?url';
import th03 from './book-covers/th03.webp?url';
import th04 from './book-covers/th04.webp?url';
import th05 from './book-covers/th05.webp?url';
import th06 from './book-covers/th06.webp?url';
import th07 from './book-covers/th07.webp?url';
import th075 from './book-covers/th075.webp?url';
import th08 from './book-covers/th08.webp?url';
import th09 from './book-covers/th09.webp?url';
import th10 from './book-covers/th10.webp?url';
import th11 from './book-covers/th11.webp?url';
import th12 from './book-covers/th12.webp?url';
import th13 from './book-covers/th13.webp?url';
import th14 from './book-covers/th14.webp?url';
import th15 from './book-covers/th15.webp?url';
import th17 from './book-covers/th17.webp?url';
import th19 from './book-covers/th19.webp?url';
import th20 from './book-covers/th20.webp?url';
import hsob from './book-covers/hsob.webp?url';

// Vite resolves the same public asset URLs for server and client rendering.
const covers: Record<string, string> = {th01, th02, th03, th04, th05, th06, th07, th075, th08, th09, th10, th11, th12, th15, th19, th20};
const singlePlayerCovers: Partial<Record<LibraryProduct['gameId'], string>> = {th13, th14, th17, hsob};
export const bookGameArtwork = (gameId: string) => covers[gameId];
export const bookArtwork = (product: LibraryProduct) =>
  (product.multiplayer ? undefined : singlePlayerCovers[product.gameId]) ?? covers[product.gameId] ?? product.artwork;
