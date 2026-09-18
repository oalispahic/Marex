'use strict';

const vscode = require('vscode');
const path = require('path');
const { formatMarex } = require('./src/format');

const TERMINAL_NAME = 'Marex';
let terminal = null;

function config(uri) {
  return vscode.workspace.getConfiguration('marex', uri);
}

// Quotes one argument for the integrated terminal's shell.
function shellQuote(arg) {
  if (process.platform === 'win32') return `"${arg.replace(/"/g, '""')}"`;
  return `'${arg.replace(/'/g, `'\\''`)}'`;
}

function marexTerminal(cwd) {
  if (terminal && terminal.exitStatus === undefined) return terminal;
  terminal = vscode.window.createTerminal({ name: TERMINAL_NAME, cwd });
  return terminal;
}

function runInTerminal(args, cwd) {
  const exe = config().get('executablePath', 'marex') || 'marex';
  const command = [exe, ...args].map(shellQuote).join(' ');
  const term = marexTerminal(cwd);
  term.show(true);
  // PowerShell needs '&' to run a quoted executable path.
  term.sendText(process.platform === 'win32' ? `& ${command}` : command);
}

// Resolves the file to run: the explorer/context argument if present,
// otherwise the active editor's document.
async function targetDocument(uri) {
  if (uri instanceof vscode.Uri) return vscode.workspace.openTextDocument(uri);
  const editor = vscode.window.activeTextEditor;
  if (editor && editor.document.languageId === 'marex') return editor.document;
  return null;
}

async function runFile(uri) {
  const document = await targetDocument(uri);
  if (!document) {
    vscode.window.showErrorMessage('Marex: open a .mx file to run it.');
    return;
  }
  if (document.isUntitled) {
    vscode.window.showErrorMessage('Marex: save the file before running it.');
    return;
  }
  if (document.isDirty && config(document.uri).get('run.saveBeforeRun', true)) {
    await document.save();
  }

  const file = document.uri.fsPath;
  const scriptArgs = config(document.uri).get('run.arguments', []);
  runInTerminal([file, ...scriptArgs], path.dirname(file));
}

function openRepl() {
  const folders = vscode.workspace.workspaceFolders;
  runInTerminal([], folders && folders.length ? folders[0].uri.fsPath : undefined);
}

function activate(context) {
  const formatter = {
    provideDocumentFormattingEdits(document, options) {
      const text = document.getText();
      const formatted = formatMarex(text, {
        insertSpaces: options.insertSpaces,
        tabSize: options.tabSize,
        collapseBlankLines: config(document.uri).get('format.collapseBlankLines', true),
      });
      if (formatted === text) return [];

      const wholeDocument = new vscode.Range(
        document.positionAt(0),
        document.positionAt(text.length)
      );
      return [vscode.TextEdit.replace(wholeDocument, formatted)];
    },
  };

  context.subscriptions.push(
    vscode.languages.registerDocumentFormattingEditProvider({ language: 'marex' }, formatter),
    vscode.commands.registerCommand('marex.runFile', runFile),
    vscode.commands.registerCommand('marex.openRepl', openRepl),
    vscode.window.onDidCloseTerminal((closed) => {
      if (closed === terminal) terminal = null;
    })
  );
}

function deactivate() {}

module.exports = { activate, deactivate };
