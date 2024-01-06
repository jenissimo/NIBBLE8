-- TextManipulation.lua
local TextManipulation = {}

function TextManipulation:appendText(textEditor, str, x, y)
    if #str == 0 then return end
    if x == nil then x = textEditor.cursor.x end
    if y == nil then y = textEditor.cursor.y end

    local linePos = textEditor.scroll.x + x
    local lineIndex = textEditor.scroll.y + y + 1
    local line = textEditor.lines[lineIndex] or ""

    -- Insert the string at the cursor position
    local beforeCursor, afterCursor, newLine
    beforeCursor = sub(line, 1, linePos)
    afterCursor = sub(line, linePos + 1)

    newLine = beforeCursor .. str .. afterCursor

    textEditor.lines[lineIndex] = newLine

    -- Update the cursor position
    local cursorOffset = #str
    textEditor:setCursor(x + cursorOffset, y)
    -- textEditor:moveCursor(cursorOffset, 0)

    textEditor.syntax_highlighting_dirty = true
end

function TextManipulation:setText(textEditor, str)
    trace("Set text: " + str)
    textEditor.text = str
    textEditor.lines = split(str, "\n")

    textEditor:setCursor(0, 0)
    textEditor.scroll.x = 0
    textEditor.scroll.y = 0
    textEditor:clearSelection()
    textEditor.syntax_highlighting_dirty = true
end

function TextManipulation:getText(textEditor)
    textEditor.text = table.concat(textEditor.lines, "\n")
    return textEditor.text
end

function TextManipulation:newLine(textEditor, x, y)
    local lineIndex = textEditor.scroll.y + y + 1
    local line = textEditor.lines[lineIndex]
    local newLine = ""

    if x == 0 then
        table.insert(textEditor.lines, lineIndex, newLine)
    else
        newLine = sub(line, 1, textEditor.scroll.x + x)
        textEditor.lines[lineIndex] = sub(line, textEditor.scroll.x + x + 1)
        table.insert(textEditor.lines, lineIndex, newLine)
    end
end

function TextManipulation:removeChar(textEditor, dir, x, y)
    local lineIndex = textEditor.scroll.y + textEditor.cursor.y + 1
    local line = textEditor.lines[lineIndex]
    local newLine = ""

    if dir == 0 or dir > 0 then return end

    if dir == -1 then
        if textEditor.scroll.x + textEditor.cursor.x == 0 then
            if textEditor.scroll.y + textEditor.cursor.y == 0 then
                return
            end
            textEditor:moveCursor(0, -1)
            textEditor:endCursor()
            newLine = textEditor.lines[lineIndex - 1] .. line
            textEditor.lines[lineIndex - 1] = newLine
            table.remove(textEditor.lines, lineIndex)
        else
            newLine =
                sub(line, 1, textEditor.scroll.x + textEditor.cursor.x - 1) ..
                    sub(line, textEditor.scroll.x + textEditor.cursor.x + 1)
            textEditor.lines[lineIndex] = newLine
            textEditor:moveCursor(dir, 0)
        end
    elseif dir == 1 then
        if textEditor.scroll.x + textEditor.cursor.x == #line then
            if textEditor.scroll.y + textEditor.cursor.y == #textEditor.lines then
                return
            end
            newLine = line .. textEditor.lines[lineIndex + 1]
            textEditor.lines[lineIndex] = newLine
            table.remove(textEditor.lines, lineIndex + 1)
        else
            newLine = sub(line, 1, textEditor.scroll.x + textEditor.cursor.x) ..
                          sub(line,
                              textEditor.scroll.x + textEditor.cursor.x + 2)
            textEditor.lines[lineIndex] = newLine
        end
    end

    textEditor.syntax_highlighting_dirty = true
end

function TextManipulation:removeSelectedText(textEditor)
    if textEditor.selection.x1 == nil then return end

    -- Invert the selection
    local startX, endX, startY, endY
    if textEditor.selection.x1 < textEditor.selection.x2 then
        startX = textEditor.selection.x1
        endX = textEditor.selection.x2
    else
        startX = textEditor.selection.x2
        endX = textEditor.selection.x1
    end
    if textEditor.selection.y1 < textEditor.selection.y2 then
        startY = textEditor.selection.y1
        endY = textEditor.selection.y2
    else
        startY = textEditor.selection.y2
        endY = textEditor.selection.y1
    end

    if startX == endX and startY == endY then
        textEditor:clearSelection()
        return
    end

    local line
    local line1
    local line2

    if endY - startY >= 1 then
        line = textEditor.lines[endY + 1]
        line1 = sub(line, 1, startX)
        line2 = sub(line, endX + 1)
        textEditor.lines[endY + 1] = line1 .. line2

        for i = endY - 1, startY, -1 do
            table.remove(textEditor.lines, i + 1)
        end
    else
        line = textEditor.lines[startY + 1]
        line1 = sub(line, 1, startX)
        line2 = sub(line, endX + 1)
        textEditor.lines[startY + 1] = line1 .. line2
    end

    textEditor:adjustCursorPositionAndScroll(startX, startY)
    trace("Cursor moved to " .. textEditor.cursor.x .. "," .. textEditor.cursor.y)
    textEditor:clearSelection()
    textEditor.syntax_highlighting_dirty = true
end

function TextManipulation:copy(textEditor)
    trace("TextManipulation:copy")

    if textEditor.selection.x1 == nil then return end

    -- Invert the selection
    local startX, endX, startY, endY
    if textEditor.selection.x1 < textEditor.selection.x2 then
        startX = textEditor.selection.x1
        endX = textEditor.selection.x2
    else
        startX = textEditor.selection.x2
        endX = textEditor.selection.x1
    end
    if textEditor.selection.y1 < textEditor.selection.y2 then
        startY = textEditor.selection.y1
        endY = textEditor.selection.y2
    else
        startY = textEditor.selection.y2
        endY = textEditor.selection.y1
    end

    if startX == endX and startY == endY then
        textEditor:clearSelection()
        return
    end

    local line
    local line1
    local line2
    local text = ""

    if endY - startY >= 1 then
        line = textEditor.lines[startY + 1]
        line1 = sub(line, startX)
        text = text .. line1 .. "\n"

        for i = startY + 1, endY do
            line = textEditor.lines[i + 1]
            if i == endY then
                text = text .. line
            else
                text = text .. line .. "\n"
            end
        end
    else
        line = textEditor.lines[startY + 1]
        text = sub(line, startX + 1, endX)
    end

    trace("Copy text: " .. text)
    set_clipboard_text(text)
end

function TextManipulation:cut(textEditor)
    textEditor:copy(textEditor)
    textEditor:removeSelectedText(textEditor)
end

function TextManipulation:paste(textEditor)
    local text = get_clipboard_text()
    if text == nil then return end
    local newLines = split(text, "\n")

    textEditor:removeSelectedText()

    -- Iterate over each line in the pasted text
    for i, lineText in ipairs(newLines) do
        if i > 1 then
            -- For subsequent lines, insert a new line
            self:newLine(textEditor, textEditor.cursor.x + 1,
                         textEditor.cursor.y)
            textEditor:moveCursor(0, 1)
            textEditor:homeCursor()
        end

        -- Append the current line of text
        self:appendText(textEditor, lineText, textEditor.cursor.x,
                        textEditor.cursor.y)
    end
end

return TextManipulation
