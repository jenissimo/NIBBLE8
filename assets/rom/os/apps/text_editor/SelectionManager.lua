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
    --textEditor:setCursor(0, 0)
    textEditor.selection.x1 = 0
    textEditor.selection.y1 = 0
    textEditor.selection.x2 = #textEditor.lines[#textEditor.lines]
    textEditor.selection.y2 = #textEditor.lines - 1
    textEditor.selection.dirX = 0
    textEditor.selection.dirY = 0
    textEditor.syntax_highlighting_dirty = true
end

function SelectionManager:selectText(textEditor)
    local dirX = textEditor.selection.dirX
    local dirY = textEditor.selection.diry
    
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
    textEditor.selection.dirX = 0
    textEditor.selection.dirY = 0
end

function SelectionManager:isSelected(textEditor, x, y)
    if textEditor.selection.x1 == nil then return false end

    local x1, x2 = textEditor.selection.x1, textEditor.selection.x2
    local y1, y2 = textEditor.selection.y1, textEditor.selection.y2

    -- Normalize y1 and y2
    if y1 > y2 then
        y1, y2 = y2, y1
    end

    -- Handle multi-line selections
    if y < y1 or y > y2 then
        return false
    elseif y == y1 and y == y2 then
        -- Single line selection
        if x1 > x2 then x1, x2 = x2, x1 end
        return x >= x1 and x <= x2
    elseif y == y1 then
        -- Starting line of multi-line selection
        return x >= x1
    elseif y == y2 then
        -- Ending line of multi-line selection
        return x <= x2
    else
        -- Middle lines of multi-line selection
        return true
    end
end

return SelectionManager
