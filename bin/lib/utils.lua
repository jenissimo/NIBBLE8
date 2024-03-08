local utils = {}

function utils.handle_text_input(key_code, ctrl_pressed, shift_pressed)
    local result = ""

    trace("key: "..str(key_code).." ctrl: "..str(ctrl_pressed).." shift: "..str(shift_pressed))

    if key_code >= 32 and key_code <= 126 then
        if shift_pressed then
            if key_code == KEYCODE.KEY_BACKQUOTE then text = text .. "~"
            elseif key_code == KEYCODE.KEY_1 then result = "!"
            elseif key_code == KEYCODE.KEY_2 then result = "@"
            elseif key_code == KEYCODE.KEY_3 then result = "#"
            elseif key_code == KEYCODE.KEY_4 then result = "$"
            elseif key_code == KEYCODE.KEY_5 then result = "%"
            elseif key_code == KEYCODE.KEY_6 then result = "^"
            elseif key_code == KEYCODE.KEY_7 then result = "&"
            elseif key_code == KEYCODE.KEY_8 then result = "*"
            elseif key_code == KEYCODE.KEY_9 then result = "("
            elseif key_code == KEYCODE.KEY_0 then result = ")"
            elseif key_code == KEYCODE.KEY_MINUS then result = "_"
            elseif key_code == KEYCODE.KEY_EQUALS then result = "+"
            elseif key_code == KEYCODE.KEY_LEFTBRACKET then result = "{"
            elseif key_code == KEYCODE.KEY_RIGHTBRACKET then result = "}"
            elseif key_code == KEYCODE.KEY_SEMICOLON then result = ":"
            elseif key_code == KEYCODE.KEY_QUOTE then result = "\""
            elseif key_code == KEYCODE.KEY_COMMA then result = "<"
            elseif key_code == KEYCODE.KEY_PERIOD then result = ">"
            elseif key_code == KEYCODE.KEY_SLASH then result = "?"
            elseif key_code == KEYCODE.KEY_BACKSLASH then result = "|"
            else
                result = chr(key_code)
            end
        else
            result = chr(key_code)
        end
    end

    return result
end

function utils.checkNumPadKeys()
    if key(KEYCODE.KEY_KP0) then appendText("0")
    elseif key(KEYCODE.KEY_KP1) then appendText("1")
    elseif key(KEYCODE.KEY_KP2) then appendText("2")
    elseif key(KEYCODE.KEY_KP3) then appendText("3")
    elseif key(KEYCODE.KEY_KP4) then appendText("4")
    elseif key(KEYCODE.KEY_KP5) then appendText("5")
    elseif key(KEYCODE.KEY_KP6) then appendText("6")
    elseif key(KEYCODE.KEY_KP7) then appendText("7")
    elseif key(KEYCODE.KEY_KP8) then appendText("8")
    elseif key(KEYCODE.KEY_KP9) then appendText("9")
    elseif key(KEYCODE.KEY_KP_PERIOD) then appendText(".")
    elseif key(KEYCODE.KEY_KP_DIVIDE) then appendText("/")
    elseif key(KEYCODE.KEY_KP_MULTIPLY) then appendText("*")
    elseif key(KEYCODE.KEY_KP_MINUS) then appendText("-")
    elseif key(KEYCODE.KEY_KP_PLUS) then appendText("+")
    elseif key(KEYCODE.KEY_KP_ENTER) then appendText("\n")
    elseif key(KEYCODE.KEY_KP_EQUALS) then appendText("=")
    end
end

return utils;