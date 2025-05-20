# Crochetify Language Support

This extension provides syntax highlighting for the Crochetify language, a domain-specific language for describing crochet patterns.

## Features

- Syntax highlighting for Crochetify keywords and patterns
- Recognition of stitches (CH, SC, DC)
- Support for color definitions (#RRGGBB format)
- Pattern definition highlighting
- Block and line comment support

## Local Installation

1. Copy this folder to `~/.vscode/extensions/` (or equivalent location for your OS)
2. Restart VS Code
3. Open a `.chfy` file or change the language mode to "Crochetify"

## Example

```crochetify
Pattern myPattern(Color col, Stitch st){
    st st st;
    TURN CH CH CH;
    col CH DC CH;
};

Color red = #FF0000;

BEGIN CROCHET
    myPattern(red, CH);
    TURN CH;
    myPattern(red, DC);
END CROCHET
```