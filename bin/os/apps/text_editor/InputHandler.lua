local InputHandler = {}

function InputHandler.handleKeyInput(textEditor, key_code, ctrl_pressed,
                                     shift_pressed)
    textEditor.selecting = false

    -- Check for Hot Keys
    if ctrl_pressed then
        if key_code == KEYCODE.KEY_A then
            trace("CTRL+A")
            textEditor:selectAll()
            return
        elseif key_code == KEYCODE.KEY_C then
            trace("CTRL+C")
            textEditor:copy()
            return
        elseif key_code == KEYCODE.KEY_X then
            trace("CTRL+X")
            textEditor:cut()
            return
        elseif key_code == KEYCODE.KEY_V then
            trace("CTRL+V")
            textEditor:paste()
            return
        elseif key_code == KEYCODE.KEY_UP then
            trace("CTRL+UP")
            textEditor.selecting = shift_pressed
            textEditor:pageUp()
            return
        elseif key_code == KEYCODE.KEY_DOWN then
            trace("CTRL+DOWN")
            textEditor.selecting = shift_pressed
            textEditor:pageDown()
            return
        elseif key_code == KEYCODE.KEY_LEFT then
            trace("CTRL+LEFT")
            textEditor.selecting = shift_pressed
            textEditor:moveCursorToLineStart()
            return
        elseif key_code == KEYCODE.KEY_RIGHT then
            trace("CTRL+RIGHT")
            textEditor.selecting = shift_pressed
            textEditor:endCursor()
            return
        end
    end

    if (key_code == KEYCODE.KEY_PAGEUP) then
        trace("PGUP")
        textEditor.selecting = shift_pressed
        textEditor:pageUp()
        return
    elseif (key_code == KEYCODE.KEY_PAGEDOWN) then
        trace("PGDN")
        textEditor.selecting = shift_pressed
        textEditor:pageDown()
        return
    end

    -- Check for Text Input
    local inputText = UTILS.handle_text_input(key_code, ctrl_pressed,
                                              shift_pressed)
    if #inputText > 0 then
        textEditor:appendText(inputText)
        return
    end

    -- Check for Cursor Movement
    if (key_code == KEYCODE.KEY_LEFT) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(-1, 0)
        return
    elseif (key_code == KEYCODE.KEY_RIGHT) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(1, 0)
        return
    elseif (key_code == KEYCODE.KEY_UP) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(0, -1)
        return
    elseif (key_code == KEYCODE.KEY_DOWN) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(0, 1)
        return
    elseif (key_code == KEYCODE.KEY_PGUP) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(0, -rows_on_screen)
        return
    elseif (key_code == KEYCODE.KEY_PGDOWN) then
        textEditor.selecting = shift_pressed
        textEditor:moveCursor(0, rows_on_screen)
        return
    elseif (key_code == KEYCODE.KEY_HOME) then
        trace("HOME")
        textEditor.selecting = shift_pressed
        textEditor:homeCursor()
        return
    elseif (key_code == KEYCODE.KEY_END) then
        trace("END")
        textEditor.selecting = shift_pressed
        textEditor:endCursor()
        return
    end

    -- Check for return
    if (key_code == KEYCODE.KEY_RETURN) then
        textEditor:newLine(textEditor.cursor.x, textEditor.cursor.y)
        textEditor:moveCursor(0, 1);
        textEditor:homeCursor();
        textEditor.syntax_highlighting_dirty = true
        return
    end

    -- Check for backspace
    if (key_code == KEYCODE.KEY_BACKSPACE) then
        trace("Backspace!")
        if textEditor.selection.x1 == nil then
            textEditor:removeChar(-1, textEditor.cursor.x, textEditor.cursor.y)
        else
            textEditor:removeSelectedText()
        end
        return
    end

    -- Check for delete
    if (key_code == KEYCODE.KEY_DELETE) then
        trace("Delete!")
        if textEditor.selection.x1 == nil then
            textEditor:removeChar(1, textEditor.cursor.x, textEditor.cursor.y)
        else
            textEditor:removeSelectedText()
        end
        return
    end

    -- Check for tab
    if (key_code == KEYCODE.KEY_TAB) then
        textEditor:appendText(" ", 0, cursor.y)
        return
    end
end

function InputHandler.handleMouseInput(textEditor, x, y, button)
    if button == 1 then
        textEditor:clearSelection()
        -- glyphs is 6x4
        -- detect cursor position based on glyph size
        -- check if cursor is pointing at character
        -- if so, set cursor position
        local desiredCursorX = math.floor((x - textEditor.x) / 4)
        local desiredCursorY = math.floor((y - textEditor.y) / 6)
        if desiredCursorY + textEditor.scroll.y > #textEditor.lines then
            return
        end

        if desiredCursorX + textEditor.scroll.x >
            #textEditor.lines[desiredCursorY + textEditor.scroll.y + 1] then
            return
        end

        textEditor:setCursor(desiredCursorX, desiredCursorY)
    end
end

return InputHandler
