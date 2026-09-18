// Pure formatting logic for Marex source, kept free of the vscode API so
// it can be unit tested with plain Node.
//
// The formatter only touches indentation and whitespace: it re-indents
// every line from the block structure (if/fi, loop/done, fun/ret, else),
// trims trailing whitespace and optionally collapses blank-line runs.
// Code inside strings and comments is never changed.

'use strict';

const OPENERS = new Set(['if', 'loop', 'fun']);
const CLOSERS = new Set(['fi', 'done']);

/**
 * Strips strings and comments from one line so keyword detection cannot
 * be fooled by text inside them. Returns the code part and whether the
 * line ends inside a block comment.
 */
function stripNonCode(line, inBlockComment) {
  let code = '';
  let i = 0;
  while (i < line.length) {
    if (inBlockComment) {
      const end = line.indexOf('*/', i);
      if (end === -1) return { code, inBlockComment: true };
      inBlockComment = false;
      i = end + 2;
      continue;
    }
    const ch = line[i];
    const next = line[i + 1];
    if (ch === '/' && next === '/') break;
    if (ch === '/' && next === '*') {
      inBlockComment = true;
      i += 2;
      continue;
    }
    if (ch === '"') {
      const close = line.indexOf('"', i + 1);
      if (close === -1) break;          // unterminated string: ignore the rest
      code += ' ';
      i = close + 1;
      continue;
    }
    code += ch;
    i++;
  }
  return { code, inBlockComment };
}

/**
 * Computes the indentation depth of each line.
 *
 * `stack` holds the open blocks. A line's depth is the stack size at its
 * start, reduced by closers that appear before any opener on that line,
 * so 'fi', 'done', 'else' and a function-closing 'ret' sit at the level
 * of the block they close while 'if (x) ret 1 fi' stays where it is.
 */
function computeDepths(lines) {
  const depths = [];
  const stack = [];
  let inBlockComment = false;

  for (const line of lines) {
    const wasInBlockComment = inBlockComment;
    const stripped = stripNonCode(line, inBlockComment);
    inBlockComment = stripped.inBlockComment;

    if (wasInBlockComment) {
      depths.push(null);               // continuation of a block comment: leave as is
      continue;
    }

    let lineDepth = stack.length;
    let seenOpener = false;
    const words = stripped.code.match(/[A-Za-z_][A-Za-z0-9_]*/g) || [];

    for (const word of words) {
      if (OPENERS.has(word)) {
        stack.push(word);
        seenOpener = true;
      } else if (CLOSERS.has(word)) {
        if (stack.length > 0) stack.pop();
        if (!seenOpener) lineDepth = Math.min(lineDepth, stack.length);
      } else if (word === 'ret') {
        if (stack.length > 0 && stack[stack.length - 1] === 'fun') {
          stack.pop();
          if (!seenOpener) lineDepth = Math.min(lineDepth, stack.length);
        }
      } else if (word === 'else') {
        if (!seenOpener && stack.length > 0) lineDepth = Math.min(lineDepth, stack.length - 1);
      }
    }
    depths.push(lineDepth);
  }
  return depths;
}

/**
 * Formats Marex source text.
 * @param {string} text
 * @param {{insertSpaces?: boolean, tabSize?: number, collapseBlankLines?: boolean}} options
 * @returns {string}
 */
function formatMarex(text, options = {}) {
  const insertSpaces = options.insertSpaces !== false;
  const tabSize = options.tabSize || 4;
  const collapseBlankLines = options.collapseBlankLines !== false;
  const unit = insertSpaces ? ' '.repeat(tabSize) : '\t';
  const eol = text.includes('\r\n') ? '\r\n' : '\n';

  const lines = text.split(/\r?\n/);
  const depths = computeDepths(lines);
  const out = [];
  let previousBlank = false;

  lines.forEach((line, index) => {
    const trimmed = line.replace(/\s+$/, '');
    const depth = depths[index];

    if (depth === null) {              // inside a block comment
      out.push(trimmed);
      previousBlank = false;
      return;
    }

    const content = trimmed.replace(/^\s+/, '');
    if (content === '') {
      if (collapseBlankLines && previousBlank) return;
      out.push('');
      previousBlank = true;
      return;
    }
    out.push(unit.repeat(depth) + content);
    previousBlank = false;
  });

  // Exactly one trailing newline.
  while (out.length > 0 && out[out.length - 1] === '') out.pop();
  return out.join(eol) + eol;
}

module.exports = { formatMarex, computeDepths, stripNonCode };
