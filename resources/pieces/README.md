# Piece SVG assets

Place optional external piece artwork here using exactly these 12 filenames:

- `white_pawn.svg` … `white_king.svg`
- `black_pawn.svg` … `black_king.svg`

They are registered in `src/qt/pieces.qrc` under the `:/pieces/` prefix.

Empty/placeholder SVGs are ignored at runtime; the Qt board falls back to Unicode chess glyphs.
