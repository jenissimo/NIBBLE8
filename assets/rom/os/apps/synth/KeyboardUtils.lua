local KeyboardUtils = {}

KeyboardUtils.keys = {
    {'white', 0}, {'black', 1}, {'white', 2}, {'black', 3}, {'white', 4},
    {'white', 5}, {'black', 6}, {'white', 7}, {'black', 8}, {'white', 9},
    {'black', 10}, {'white', 11}, {'white', 12}, {'black', 13}, {'white', 14},
    {'black', 15}, {'white', 16}, {'white', 17}, {'black', 18}, {'white', 19},
    {'black', 20}, {'white', 21}, {'black', 22}, {'white', 23}
}

--
--  1 3   6 8 10   13
-- 0 2 4 5 7 9 11 12
KeyboardUtils.keyboardToPiano = {
    -- First octave white keys
    [KEYCODE.KEY_Z] = KeyboardUtils.keys[1][2],
    [KEYCODE.KEY_S] = KeyboardUtils.keys[2][2],
    [KEYCODE.KEY_X] = KeyboardUtils.keys[3][2],
    [KEYCODE.KEY_D] = KeyboardUtils.keys[4][2],
    [KEYCODE.KEY_C] = KeyboardUtils.keys[5][2],
    [KEYCODE.KEY_V] = KeyboardUtils.keys[6][2],
    [KEYCODE.KEY_G] = KeyboardUtils.keys[7][2],
    [KEYCODE.KEY_B] = KeyboardUtils.keys[8][2],
    [KEYCODE.KEY_H] = KeyboardUtils.keys[9][2],
    [KEYCODE.KEY_N] = KeyboardUtils.keys[10][2],
    [KEYCODE.KEY_J] = KeyboardUtils.keys[11][2],
    [KEYCODE.KEY_M] = KeyboardUtils.keys[12][2],
    [KEYCODE.KEY_Q] = KeyboardUtils.keys[13][2],
    [KEYCODE.KEY_2] = KeyboardUtils.keys[14][2],
    [KEYCODE.KEY_W] = KeyboardUtils.keys[15][2],
    [KEYCODE.KEY_3] = KeyboardUtils.keys[16][2],
    [KEYCODE.KEY_E] = KeyboardUtils.keys[17][2],
    [KEYCODE.KEY_R] = KeyboardUtils.keys[18][2],
    [KEYCODE.KEY_5] = KeyboardUtils.keys[19][2],
    [KEYCODE.KEY_T] = KeyboardUtils.keys[20][2],
    [KEYCODE.KEY_6] = KeyboardUtils.keys[21][2],
    [KEYCODE.KEY_Y] = KeyboardUtils.keys[22][2],
    [KEYCODE.KEY_7] = KeyboardUtils.keys[23][2],
    [KEYCODE.KEY_U] = KeyboardUtils.keys[24][2]
}

KeyboardUtils.noteNames = {
    "c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"
}

function KeyboardUtils.keyNumberToNoteName(keyNumber, octave)
    local noteIndex = (keyNumber % 12) + 1
    local noteName = KeyboardUtils.noteNames[noteIndex]
    if octave ~= nil then
        local octaveOffset = math.floor(keyNumber / 12)
        return noteName .. (octave + octaveOffset)
    end
    return noteName
end

function KeyboardUtils.keyCodeToPiano(key_code)
    return (KeyboardUtils.keyboardToPiano[key_code] % 12) + 1
end

return KeyboardUtils
