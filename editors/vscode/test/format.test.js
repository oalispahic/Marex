'use strict';

const { test } = require('node:test');
const assert = require('node:assert/strict');
const { formatMarex } = require('../src/format');

const fmt = (lines, options) => formatMarex(lines.join('\n') + '\n', options);
const lines = (text) => text.split('\n');

test('re-indents if/else/fi and loops', () => {
  const input = ['if (x)', 'print(1)', 'else', 'loop 0 -> 3', 'print(2)', 'done', 'fi', ''];
  assert.deepEqual(lines(fmt(input)), [
    'if (x)',
    '    print(1)',
    'else',
    '    loop 0 -> 3',
    '        print(2)',
    '    done',
    'fi',
    '',
  ]);
});

test('ret closes a function body but is an early return inside a block', () => {
  const input = [
    'fun sign(n)',
    'if (n < 0) ret -1 fi',
    'if (n > 0)',
    'ret 1',
    'fi',
    'ret 0',
    'fun hello()',
    'print("hi")',
    'ret',
    'print(sign(3))',
    '',
  ];
  assert.deepEqual(lines(fmt(input)), [
    'fun sign(n)',
    '    if (n < 0) ret -1 fi',
    '    if (n > 0)',
    '        ret 1',
    '    fi',
    'ret 0',
    'fun hello()',
    '    print("hi")',
    'ret',
    'print(sign(3))',
    '',
  ]);
});

test('one-line blocks keep their level', () => {
  const input = ['loop (var i := 0; i < 3; i++) print(i) done', 'print(newln)', ''];
  assert.deepEqual(lines(fmt(input)), input);
});

test('ignores keywords inside strings and comments', () => {
  const input = [
    'var s := "if loop fun"   // done fi',
    '/* fun',
    '     keep me */',
    'print(s)',
    '',
  ];
  assert.deepEqual(lines(fmt(input)), [
    'var s := "if loop fun"   // done fi',
    '/* fun',
    '     keep me */',
    'print(s)',
    '',
  ]);
});

test('trims trailing whitespace, collapses blank runs, honours tabs', () => {
  const input = ['fun f()   ', '', '', '', 'print(1)\t', 'ret', '', '', ''];
  assert.deepEqual(lines(fmt(input, { insertSpaces: false })), [
    'fun f()',
    '',
    '\tprint(1)',
    'ret',
    '',
  ]);
  assert.deepEqual(lines(fmt(input, { collapseBlankLines: false, tabSize: 2 })), [
    'fun f()',
    '',
    '',
    '',
    '  print(1)',
    'ret',
    '',
  ]);
});

test('preserves CRLF line endings', () => {
  const out = formatMarex('if (1)\r\nprint(1)\r\nfi\r\n');
  assert.equal(out, 'if (1)\r\n    print(1)\r\nfi\r\n');
});

test('formatting the shipped examples is idempotent and parses the same', () => {
  const fs = require('node:fs');
  const path = require('node:path');
  const dir = path.join(__dirname, '..', '..', '..', 'test_program');
  for (const file of fs.readdirSync(dir).filter((f) => f.endsWith('.mx'))) {
    const source = fs.readFileSync(path.join(dir, file), 'utf8');
    const once = formatMarex(source);
    assert.equal(formatMarex(once), once, `${file} is not stable`);
    // Whitespace-insensitive equality: the formatter must not change tokens.
    assert.equal(once.replace(/\s+/g, ' '), source.replace(/\s+/g, ' ').replace(/\s*$/, ' '), `${file} changed`);
  }
});
