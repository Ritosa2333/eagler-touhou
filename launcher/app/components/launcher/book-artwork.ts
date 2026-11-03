import type {LibraryProduct} from './products';
import th06 from './book-covers/th06.webp?url';
import th07 from './book-covers/th07.webp?url';
import th075 from './book-covers/th075.webp?url';
import th08 from './book-covers/th08.webp?url';
import th09 from './book-covers/th09.webp?url';
import th10 from './book-covers/th10.webp?url';
import th11 from './book-covers/th11.webp?url';
import th12 from './book-covers/th12.webp?url';
import th15 from './book-covers/th15.webp?url';
import th20 from './book-covers/th20.webp?url';

// Vite resolves the same public asset URLs for server and client rendering.
const covers: Record<string, string> = {th06, th07, th075, th08, th09, th10, th11, th12, th15, th20};
export const bookGameArtwork = (gameId: string) => covers[gameId];
export const bookArtwork = (product: LibraryProduct) => covers[product.gameId] ?? product.artwork;
