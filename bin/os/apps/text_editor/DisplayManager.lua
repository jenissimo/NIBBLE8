local DisplayManager = {}

function DisplayManager:drawEditor(textEditor)
    rectfill(textEditor.x - 1, textEditor.y - 1, textEditor.width + 1,
             textEditor.height + 1, 0)
    if textEditor.cursor.visible == true then textEditor:drawCursor() end
    self:redrawText(textEditor)
    self:drawStatusBar(textEditor)
end

function DisplayManager:redrawText(textEditor)
    local firstLineIndex = math.max(1, math.floor(textEditor.scroll.y) + 1)
    local lastLineIndex = firstLineIndex + textEditor.rows_on_screen - 1
    local startY = textEditor.y + textEditor.offsetY
    local lineHeight = 6 -- Height of each line of text
    local charWidth = 4 -- Width of each character
    local xPos = textEditor.x - textEditor.scroll.x * charWidth
    local index = 1

    for lineIndex = firstLineIndex, lastLineIndex do
        local yOffset = (lineIndex - firstLineIndex) * lineHeight + startY
        local drawText = textEditor.drawLines[index]
        --local cursorDocumentY = textEditor.cursor.y + textEditor.scroll.y
        --if lineIndex == cursorDocumentY + 1 then
            --drawText = textEditor:cacheLine(lineIndex)
        --end
        if drawText == nil then break end
        print(drawText, xPos, yOffset)
        index = index + 1
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
