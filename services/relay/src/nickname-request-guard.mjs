import {isIP} from 'node:net';

// Rotating the host part of an IPv6 address must not create a fresh allowance.
export function nicknameAddressScope(address) {
  if (isIP(address) !== 6) return address;
  const canonical = new URL(`http://[${address.split('%')[0]}]/`).hostname.slice(1, -1);
  const [left, right] = canonical.split('::');
  const head = left ? left.split(':') : [], tail = right ? right.split(':') : [];
  const groups = right === undefined ? head : [...head, ...Array(8-head.length-tail.length).fill('0'), ...tail];
  const words = groups.map(word => parseInt(word, 16));
  if (words.slice(0, 5).every(word => word === 0) && words[5] === 65535)
    return [words[6] >> 8, words[6] & 255, words[7] >> 8, words[7] & 255].join('.');
  return 'v6:' + words.slice(0, 4).map(word => word.toString(16)).join(':');
}

// Cheap admission before any worker RPC or SQLite work. Bounded across reconnects.
export function createNicknameRequestGuard({now = Date.now, maxAddresses = 10000} = {}) {
  const addresses = new Map();
  let sweepAt = 0;
  return address => {
    const key = nicknameAddressScope(address), time = now();
    let row = addresses.get(key);
    if (!row || row.until <= time) {
      if (!row && addresses.size >= maxAddresses) {
        if (time >= sweepAt) {
          for (const [key, value] of addresses) if (value.until <= time) addresses.delete(key);
          sweepAt = time + 1000;
        }
        if (addresses.size >= maxAddresses) return time + 10000;
      }
      row = {count:0, until:time + 10000};
      addresses.set(key, row);
    }
    return ++row.count <= 10 ? 0 : row.until;
  };
}
