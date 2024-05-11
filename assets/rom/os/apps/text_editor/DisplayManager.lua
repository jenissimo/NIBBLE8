local DisplayManager = {}

function DisplayManager:drawEditor(textEditor)
    rectfill(textEditor.x - 1, textEditor.y - 1, textEditor.width + 1,
             textEditor.height + 1, 0)
    self:redrawText(textEditor)
    if textEditor.cursor.visible == true then textEditor:drawCursor() end
    self:drawStatusBar(textEditor)
end

function DisplayManager:redrawText(textEditor)
    local startY = textEditor.y + textEditor.offsetY
    local charWidth = 4 -- Width of each character
    local xPos = textEditor.x - textEditor.scroll.x * charWidth

    print(textEditor.drawText, xPos, startY)
end

function DisplayManager:drawStatusBar(textEditor)
    local status = "line: " ..
                       str(flr(textEditor.cursor.y + textEditor.scroll.y + 1)) ..
                       "/" .. str(#textEditor.lines)
    local s = textEditor.selection

    if s.x1 ~= nil then
        status = string.format("selection %d,%d:%d,%d", s.x1, s.y1, s.x2, s.y2)
    end

    rectfill(0, 113, 160, 7, 2)
    print(status, 1, 114, 0)
end

return DisplayManager
