import {writeFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';

const [, , contract, output] = process.argv;
const {QUICK_CHAT_PHRASES} = await import(pathToFileURL(contract).href);
const entries = QUICK_CHAT_PHRASES.map(({id, zh}) => '    {' + JSON.stringify(id) + ', ' + JSON.stringify(zh) + '},');
await writeFile(output, [
  '#pragma once',
  '#include <array>',
  '#include <string_view>',
  'namespace th09::desktop {',
  'struct QuickPhrase { std::string_view id, text; };',
  'inline constexpr std::array<QuickPhrase, ' + entries.length + '> quickPhrases{{',
  ...entries,
  '}};',
  '}',
  '',
].join('\n'));
