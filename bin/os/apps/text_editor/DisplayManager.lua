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
    local startY = textEditor.y + textEditor.offsetY

    for lineIndex, drawText in ipairs(textEditor.drawLines) do
        local lineHeight = 6 -- Height of each line of text
        local charWidth = 4 -- Width of each character
        local yOffset = (lineIndex - firstLineIndex) * lineHeight + startY
        local xPos = textEditor.x - textEditor.scroll.x * charWidth

        print(drawText, xPos, yOffset)
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
