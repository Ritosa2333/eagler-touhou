import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {parse} from 'parse5';
import {PRODUCT_IDS, PRODUCT_GAMES, productEnabledForBuild, gameIdForProduct, isMultiplayerProductId} from '../lib/contracts/product-catalog.mjs';
import {APP_SHELL_FILES, FRONTEND_PACKAGE_FILES, resolveFrontendPackageSource} from '../lib/frontend-manifest.mjs';
import {createLibraryProducts} from '../app/components/launcher/products.ts';
import {bookArtwork} from '../app/components/launcher/book-artwork.ts';
import {hostArtworkFiles} from '../lib/host-artwork.mjs';

const project = fileURLToPath(new URL('..', import.meta.url));
const attr = (node, name) => node.attrs?.find(value => value.name === name)?.value ?? null;
const nodes = root => [root, ...(root.childNodes ?? []).flatMap(nodes)];
const sha = bytes => createHash('sha256').update(bytes).digest('hex');

test('published home registers visible single-player products, routes and complete offline covers', async () => {
  const document = parse(await readFile(resolveFrontendPackageSource('index.html'), 'utf8'));
  const grid = nodes(document).find(node => attr(node, 'id') === 'bookGameGrid');
  assert.ok(grid, 'published home includes the production game library');
  const links = nodes(grid).filter(node => node.tagName === 'a' && attr(node, 'data-book-product'));
  const expected = PRODUCT_IDS.filter(id => !isMultiplayerProductId(id) && productEnabledForBuild(id));
  assert.deepEqual(links.map(node => attr(node, 'data-book-product')).sort(), [...expected].sort());
  assert.equal(new Set(links.map(node => attr(node, 'data-book-product'))).size, links.length);
  const products = createLibraryProducts(expected, path => path, id => '?game=' + id);
  for (const link of links) {
    const id = attr(link, 'data-book-product'), product = products.find(item => item.id === id);
    assert.ok(product);
    assert.equal(attr(link, 'data-game'), gameIdForProduct(id));
    assert.equal(new URL(attr(link, 'href'), 'https://launcher.invalid/').searchParams.get('game'), id);
    const images = nodes(link).filter(node => node.tagName === 'img');
    const artwork = bookArtwork(product);
    if (artwork) {
      assert.equal(images.length, 1, id + ': one cover per card');
      const published = attr(images[0], 'src').replace(/^\//, '');
      if (FRONTEND_PACKAGE_FILES.includes(published)) {
        assert.ok(APP_SHELL_FILES.includes(published), id + ': offline bundled cover');
        assert.equal(sha(await readFile(resolveFrontendPackageSource(published))),
          sha(await readFile(resolve(project, artwork.replace(/^\//, '')))), id + ': correct artwork bytes');
      } else {
        assert.equal(published, product.artwork, id + ': host cover follows the catalog');
        assert.ok(hostArtworkFiles([gameIdForProduct(id)]).includes(published.replace(/^assets\//, '')), id + ': declared host artwork');
      }
    } else assert.equal(images.length, 0, id + ': no missing-artwork request');
  }
});

test('every normal and multiplayer product preserves catalog identity and routing metadata', () => {
  const products = createLibraryProducts(PRODUCT_IDS, path => path, id => '?game=' + id);
  assert.deepEqual(products.map(item => item.id), [...PRODUCT_IDS]);
  for (const product of products) {
    const game = PRODUCT_GAMES[gameIdForProduct(product.id)];
    assert.equal(product.gameId, gameIdForProduct(product.id));
    assert.equal(product.multiplayer, isMultiplayerProductId(product.id));
    assert.equal(product.href, '?game=' + product.id);
    assert.equal(product.title, game.title);
    assert.equal(product.number, game.number);
    assert.equal(product.sourceRepository, game.support.sourceRepository);
    assert.equal(product.artwork, game.cardArtwork ? 'assets/' + game.cardArtwork : null);
    assert.deepEqual(product.presentation, game.cardPresentation);
  }
});
