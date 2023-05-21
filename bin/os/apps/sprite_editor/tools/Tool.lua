local Tool = {}
Tool.__index = Tool

function Tool.new()
    local self = setmetatable({}, Tool)
    self.oldPixels = {}
    self.newPixels = {}
    return self
end

function Tool:isDirty()
    local dirty = false

    for y = 1, #self.newPixels do
        for x = 1, #self.newPixels[y] do
            if self.newPixels[y][x] ~= self.oldPixels[y][x] then
                dirty = true
                break
            end
        end
    end

    return dirty
end

function Tool:initPixels(drawArea)
    local sprSize = drawArea:getSpriteSize()
    self.oldPixels = {}
    self.newPixels = {}
    for y = 1, sprSize do
        self.oldPixels[y] = {}
        self.newPixels[y] = {}
        for x = 1, sprSize do
            self.oldPixels[y][x] = drawArea:getPixelLocal(x - 1, y - 1)
            self.newPixels[y][x] = drawArea:getPixelLocal(x - 1, y - 1)
        end
    end
end

function Tool:isCursorInBounds(drawArea, cursor)
    local sprSize = drawArea:getSpriteSize()
    if cursor.x < 0 or cursor.x >= sprSize or cursor.y < 0 or cursor.y >= sprSize then
        return false
    end
    return true
end

function Tool:setPixel(drawArea, x, y)
    local cursor = drawArea:getCursorPos(x, y)
    if self:isCursorInBounds(drawArea, cursor) then
        drawArea:setPixel(x, y)
        self.newPixels[cursor.y + 1][cursor.x + 1] = drawArea:getPixel(x, y)
    end
end

function Tool:resetPixels(drawArea)
    local sprSize = drawArea:getSpriteSize()
    for y = 1, sprSize do
        for x = 1, sprSize do
            self.newPixels[y][x] = self.oldPixels[y][x]
        end
    end
end

function Tool:onMouseDown(drawArea, x, y)
end

function Tool:onMouseUp(drawArea, x, y)
end

function Tool:onMouseMove(drawArea, x, y)
end

return Tool
