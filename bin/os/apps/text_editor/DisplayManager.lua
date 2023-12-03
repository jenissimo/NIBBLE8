local DisplayManager = {}

function DisplayManager:drawEditor(textEditor)
    rectfill(textEditor.x - 1, textEditor.y - 1, textEditor.width + 1,
             textEditor.height + 1, 0)
    if textEditor.cursor.visible == true then textEditor:drawCursor() end
    self:redrawText(textEditor)
    self:drawStatusBar(textEditor)
end

function DisplayManager:redrawText(textEditor)
    local lineHeight = 6 -- Height of each line of text
    local charWidth = 4 -- Width of each character

    -- Calculate the vertical offset to start drawing from, based on the scroll position.
    local startY = textEditor.y + textEditor.offsetY

    -- Determine the range of lines to draw, considering partial visibility.
    local firstLineIndex = math.max(1, math.floor(textEditor.scroll.y) + 1)
    local lastLineIndex = firstLineIndex + textEditor.rows_on_screen - 1

    -- Ensure we don't try to draw more lines than exist.
    lastLineIndex = math.min(lastLineIndex, #textEditor.lines)

    -- Loop through each line that should be visible.
    for lineIndex = firstLineIndex, lastLineIndex do
        local line = textEditor.lines[lineIndex] or ""
        local yOffset = (lineIndex - firstLineIndex) * lineHeight + startY

        -- Loop through each character in the line.
        for charIndex = 1, #line do
            local char = line:sub(charIndex, charIndex)
            local color, bgColor = textEditor:getColor(lineIndex, charIndex), 0

            -- Check if the character is part of a selection.
            if textEditor:isSelected(charIndex - 1, lineIndex - 1) then
                color = 3
                bgColor = 1
            elseif textEditor:isUnderCursor(charIndex - 1, lineIndex - 1) and textEditor.cursor.visible then
                color = 3
            end

            -- Calculate the X position for this character.
            local xPos = textEditor.x + (charIndex - 1 - textEditor.scroll.x) *
                             charWidth

            -- Draw the character at the calculated position.
            print(char, xPos, yOffset, color, bgColor)
        end
    end
end

function DisplayManager:drawStatusBar(textEditor)
    local status = "line: " ..
                       str(flr(textEditor.cursor.y + textEditor.scroll.y + 1)) ..
                       "/" .. str(#textEditor.lines)

    rectfill(0, 113, 160, 7, 2)
    print(status, 1, 114, 0)
end

return DisplayManager
