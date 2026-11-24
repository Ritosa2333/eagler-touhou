import {useState} from 'react';
import {Gamepad2} from 'lucide-react';
import {BookWorkNumber} from './BookWorkNumber';
import {bookArtwork} from './book-artwork';
import type {LibraryProduct} from './products';

export function BookRailGameIdentity({product}: {product: LibraryProduct}) {
  const source = bookArtwork(product);
  return <>
    <RailCover key={source} source={source}/>
    <span className="book-category-game-identity">
      <span className="book-category-game-name">
        <span className="book-category-game-title" title={product.title}>{product.title}</span>
        <BookWorkNumber gameId={product.gameId} number={product.number}/>
      </span>
      {product.subtitle && <span className="book-category-game-subtitle" title={product.subtitle}>{product.subtitle}</span>}
    </span>
  </>;
}

function RailCover({source}: {source: ReturnType<typeof bookArtwork>}) {
  const [failed, setFailed] = useState(false);
  return <span className="book-category-game-cover" aria-hidden="true">
    {source && !failed ? <img src={source} alt="" loading="lazy" decoding="async" onError={() => setFailed(true)}/> : <Gamepad2/>}
  </span>;
}
