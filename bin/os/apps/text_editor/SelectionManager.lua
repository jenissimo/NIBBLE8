local SelectionManager = {}

function SelectionManager:startSelection(textEditor)
    textEditor.selection.x1 = textEditor.cursor.x + textEditor.scroll.x
    textEditor.selection.y1 = textEditor.cursor.y + textEditor.scroll.y
    textEditor.selection.x2 = textEditor.cursor.x + textEditor.scroll.x
    textEditor.selection.y2 = textEditor.cursor.y + textEditor.scroll.y
end

-- TODO: Rewrite check for shift
function SelectionManager:checkSelectionStart(textEditor)
    if textEditor.selecting then
        if textEditor.selection.x1 == nil then
            self:startSelection(textEditor)
        end
    else
        self:clearSelection(textEditor)
    end
end

function SelectionManager:checkSelectionUpdate(textEditor)
    if textEditor.selecting then self:selectText(textEditor) end
end

function SelectionManager:selectAll(textEditor)
    textEditor:setCursor(0, 0)
    textEditor.selection.x1 = 0
    textEditor.selection.y1 = 0
    textEditor.selection.x2 = #textEditor.lines[#textEditor.lines]
    textEditor.selection.y2 = #textEditor.lines - 1
end

function SelectionManager:selectText(textEditor)
    local dirX = 0
    local dirY = 0

    if key(KEYCODE.KEY_LEFT) == 1 then
        dirX = -1
    elseif key(KEYCODE.KEY_RIGHT) == 1 then
        dirX = 1
    elseif key(KEYCODE.KEY_UP) == 1 then
        dirY = -1
    elseif key(KEYCODE.KEY_DOWN) == 1 then
        dirY = 1
    elseif key(KEYCODE.KEY_HOME) == 1 then
        dirX = -1
    elseif key(KEYCODE.KEY_END) == 1 then
        dirX = 1        
    end

    if dirX ~= 0 or dirY ~= 0 then
        if textEditor.selection.x1 == nil then
            self:startSelection(textEditor)
        else
            textEditor.selection.x2 = textEditor.cursor.x + textEditor.scroll.x
            textEditor.selection.y2 = textEditor.cursor.y + textEditor.scroll.y
        end
    end

    -- trace cursor pos
    --trace("x: " .. textEditor.cursor.x .. " y: " .. textEditor.cursor.y)
    --trace("x1: " .. textEditor.selection.x1 .. " x2: " .. textEditor.selection.x2)
    --trace("y1: " .. textEditor.selection.y1 .. " y2: " .. textEditor.selection.y2)

    if textEditor.selection.x2 < 0 or textEditor.selection.y2 < 0 then
        self:clearSelection(textEditor)
    end
end

function SelectionManager:clearSelection(textEditor)
    textEditor.selection.x1 = nil
    textEditor.selection.y1 = nil
    textEditor.selection.x2 = nil
    textEditor.selection.y2 = nil
end

function SelectionManager:isSelected(textEditor, x, y)
    if textEditor.selection.x1 == nil then return false end

    local x1 = textEditor.selection.x1
    local x2 = textEditor.selection.x2
    local y1 = textEditor.selection.y1
    local y2 = textEditor.selection.y2

    if textEditor.selection.x1 > textEditor.selection.x2 then
        x1 = textEditor.selection.x2
        x2 = textEditor.selection.x1 - 1
    else
        x2 = textEditor.selection.x2 - 1
    end

    if textEditor.selection.y1 > textEditor.selection.y2 then
        y1 = textEditor.selection.y2
        y2 = textEditor.selection.y1
    end

    if y == y1 and y == y2 then
        if x >= x1 and x <= x2 then return true end
    elseif y == y1 then
        if x >= x1 then return true end
    elseif y == y2 then
        if x <= x2 then return true end
    elseif y > y1 and y < y2 then
        return true
    end

    return false
end

return SelectionManager
