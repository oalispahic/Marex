'use strict';

const vscode = require('vscode');
const { formatMarex } = require('./src/format');

function activate(context) {
  const provider = {
    provideDocumentFormattingEdits(document, options) {
      const config = vscode.workspace.getConfiguration('marex', document.uri);
      const text = document.getText();
      const formatted = formatMarex(text, {
        insertSpaces: options.insertSpaces,
        tabSize: options.tabSize,
        collapseBlankLines: config.get('format.collapseBlankLines', true),
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
    vscode.languages.registerDocumentFormattingEditProvider({ language: 'marex' }, provider)
  );
}

function deactivate() {}

module.exports = { activate, deactivate };
