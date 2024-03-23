NIBBLE-8 API Reference
Resolution: 160x120 pixels
Color Palette: 4 colors
Programming Language: Lua
Graphics
print(str, [x, y, col]): Draw text on the screen.
camera([x,] [y]): Set the camera's position.
pal(c0, c1, [p]): Replace one color with another in the current palette.
cpal([color, r, g, b] | [invert]): Change colors in the palette or invert the color palette. If called with a single boolean argument (invert), it inverts the entire color palette if true. When called with color index and RGB values (color, r, g, b), it changes the specified color in the palette to the new RGB value provided.
palt(c, t): Set transparency for a color.
pget(x, y): Get the color of a pixel.
pset(x, y, col): Set the color of a pixel.
circ(x, y, r, col): Draw a circle.
circfill(x, y, r, col): Draw a filled circle.
line(x0, y0, x1, y1, col): Draw a line.
rect(x0, y0, width, height, col): Draw a rectangle.
rectfill(x0, y0, width, height, col): Draw a filled rectangle.
cls([col]): Clear the screen with a color.
Sprite Functions
spr(n, x, y, [w, h, flip_x, flip_y]): Draw a sprite.
sspr(sx, sy, sw, sh, dx, dy, [dw, dh, flip_x, flip_y]): Draw a scaled sprite.
sset(x, y, col): Set a pixel in the sprite sheet.
sget(x, y): Get a pixel from the sprite sheet.
Map Functions
map(celx, cely, sx, sy, celw, celh, [layer]): Draw a map section.
mget(x, y): Get a map value.
mset(x, y, v): Set a map value.
Utility Functions
split(str, [delimiter]): Split a string by a delimiter.
trace(msg): Output a debug message.
rnd(x): Get a random number.
chr(n): Convert a number to a character.
time(): Get the current time.
flr(x), ceil(x), sin(x), cos(x), atan2(y, x), mid(x, y, z), min(x, y), max(x, y): Math functions.
Input
key(k), keyp(k): Keyboard input.
btn(b), btnp(b): Gamepad input.
Clipboard
get_clipboard_text(): Get text from the clipboard.
set_clipboard_text(str): Set text to the clipboard.
File Management
load_file(filename), read_file(filename), ls(), cd(dir), load_cart(cart), save_cart(cart, data), import_png(filename), export_png(filename): File and directory operations.
Audio
note_on(note, octave, instrument, volume): Start playing a note.
note_off(note, octave): Stop playing a note.
update_synth(osc, attacktime, decaytime, sustainamplitude, releasetime, startamplitude): Update synthesizer parameters.
update_filter(filter, value): Update audio filter settings.
