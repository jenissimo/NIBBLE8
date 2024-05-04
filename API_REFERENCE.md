# NIBBLE8 API Reference

## Buttons constants for btn and btnp
| Keyboard Key   | Value | Description           |
|----------------|-------|-----------------------|
| Left           | 0     | Left button           |
| Right          | 1     | Right button          |
| Up             | 2     | Up button             |
| Down           | 3     | Down button           |
| Z              | 4     | A button              |
| X              | 5     | B button              |
| Enter          | 6     | Start button          |
| Right Shift    | 7     | Select button         |

## Callback Functions

- `_init():` Initialization function, run once on start
- `_update():` Update function, runs 30 frames per second
- `_draw():` Draw function, runs 30 frames per second
- `_key(key_code, ctrl_pressed, shift_pressed):` Key press event handler.
- `_keyup(key_code, ctrl_pressed, shift_pressed):` Key release event handler.
- `_mousep(x, y, button):` Mouse press event handler.
- `_mouser(x, y, button):` Mouse release event handler.
- `_mousem(x, y):` Mouse movement event handler.

## Low Level Memory Functions

- `peek(addr):` Read a byte from memory.
- `poke(addr, value):` Write a byte to memory.
- `peek2(addr):` Read a 16-bit value from memory.
- `poke2(addr, value):` Write a 16-bit value to memory.

## Graphics Functions

- `print(str, [x], [y], [fgcol], [bg]):` Draw text on the screen.
- `cursor(x, y):` Set the cursor position.
- `color(col):` Set the drawing color.
- `camera(x, y):` Set the camera's position.
- `cpal(c, r, g, b):` Replace a color in the palette.
- `pal(c0, c1):` Swaps colour c0 for c1.
- `palt([c], [t]):` Set transparency for a color. If empty - resets.
- `pget(x, y):` Get the color of a pixel.
- `pset(x, y, col):` Set the color of a pixel.
- `circ(x, y, r, col):` Draw a circle.
- `circfill(x, y, r, col):` Draw a filled circle.
- `line(x0, y0, x1, y1, col):` Draw a line.
- `rect(x0, y0, width, height, col):` Draw a rectangle.
- `rectfill(x0, y0, width, height, col):` Draw a filled rectangle.
- `cls([col]):` Clear the screen with a color.

## Sprite Functions

- `spr(n, x, y, [flip_x], [flip_y]):` Draw a sprite.
- `sspr(sx, sy, sw, sh, dx, dy, [dw], [dh], [flip_x], [flip_y]):` Draw a scaled sprite.
- `sset(x, y, col):` Set a pixel in the sprite sheet.
- `sget(x, y):` Get a pixel from the sprite sheet.
- `fget(n, f):` Get a flag from a sprite.
- `fset(n, f, v):` Set a flag for a sprite.

## Map Functions

- `map(celx, cely, sx, sy, celw, celh, [layer]):` Draw a map section.
- `mget(x, y):` Get a map value.
- `mset(x, y, v):` Set a map value.

## Utility Functions

- `add(t, a):` Add an element to a Lua table.
- `del(t, a):` Remove an element from a Lua table.
- `str(a):` Convert a value to a string.
- `len(a):` Get the length of a string.
- `sub(str, str_start, str_end):` Extract a substring.
- `rnd(x):` Get a random number.
- `chr(n):` Convert a number to a character.
- `time():` Get the current time.

## Math Functions
- `flr(x)`: Returns the largest integer less than or equal to x. It effectively rounds x down to the nearest integer.
- `ceil(x)`: Returns the smallest integer greater than or equal to x. It effectively rounds x up to the nearest integer.
- `sin(x)`: Returns the sine of x, where x is an angle in radians. The sine function describes the ratio of the length of the side opposite an angle to the length of the hypotenuse in a right-angled triangle.
- `cos(x)`: Returns the cosine of x, where x is an angle in radians. The cosine function describes the ratio of the length of the adjacent side to the length of the hypotenuse in a right-angled triangle.
- `atan2(y, x)`: Returns the angle whose tangent is the quotient of y and x. This function is useful for calculating angles in radians, especially when you have both the y and x components of a vector.
- `mid(x, y, z)`: Returns the middle value among x, y, and z. It is useful for finding the median value among a set of numbers.
- `min(x, y)`: Returns the smaller of the two values x and y. It is useful for finding the minimum value among two numbers.
- `max(x, y)`: Returns the larger of the two values x and y. It is useful for finding the maximum value among two numbers.

## Input Functions

- `key(key_code):` Check if a key is pressed.
- `keyp(key_code):` Check if a key was pressed this frame.
- `btn(button):` Check if a gamepad button is pressed.
- `btnp(button):` Check if a gamepad button was pressed this frame.

## Clipboard Functions

- `get_clipboard_text():` Get text from the clipboard.
- `set_clipboard_text(str):` Set text to the clipboard.

## File Management Functions

- `load_file(filename), read_file(filename), ls(), cd(dir), load_cart(cart), save_cart(cart, data), import_png(filename), export_png(filename):` File and directory operations.

## System Functions

- `reboot():` Reboot the system.
- `get_code():` Get the error code.
- `run_code(code):` Run a Lua code snippet.
- `exit():` Exit the application.

