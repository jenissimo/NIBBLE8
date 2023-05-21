-- Draw Area
local UIElement = require("ui/UIElement")
local DrawArea = setmetatable({}, UIElement)
DrawArea.__index = DrawArea

local PenTool = require("os/apps/sprite_editor/tools/PenTool")

function DrawArea.new(x, y, width, height, commandStack)
    local self = setmetatable(UIElement.new(x, y, width, height), DrawArea)
    self.currentSprite = 0
    self.currentColor = 0
    self.drawing = 0
    self.currentTool = PenTool.new()
    self.commandStack = commandStack
    self:setZoom(1)

    return self
end

function DrawArea:setZoom(zoom)
    self.zoom = zoom
    self.zoomCoef = self.width / (self.zoom * 8)
end

function DrawArea:draw()
    rect(self.x - 1, self.y - 1, self.width + 2, self.height + 2, 0)
    -- top border
    line(self.x - 1, self.y - 2, self.x + self.width, self.y - 2, 2)

    -- left border
    line(self.x - 2, self.y - 1, self.x - 2, self.y + self.height, 2)

    -- right border
    line(self.x + self.width + 1, self.y - 1, self.x + self.width + 1, self.y + self.height, 3)

    -- bottom border
    line(self.x - 1, self.y + self.height + 1, self.x + self.width, self.y + self.height + 1, 3)

    rectfill(self.x, self.y, self.width, self.height, 1)

    for i = 0, 3 do
        palt(0, false)
    end

    local sprites_per_row = 20;
    local row = flr(self.currentSprite / sprites_per_row);
    local col = self.currentSprite % sprites_per_row;

    -- trace("Current sprite: "..self.currentSprite.." - "..row.." - "..col)

    -- Draw sprite using sspr
    sspr(col * 8, row * 8, 8 * self.zoom, 8 * self.zoom, self.x, self.y, self.width, self.height)

    palt()
end

function DrawArea:update(dt)
    if self.drawing ~= 1 then
        return
    end

    if self.cursorX < self.x or self.cursorX > self.x + self.width or self.cursorY < self.y or self.cursorY > self.y +
        self.height then
        return
    end

    -- self:setPixel(self.cursorX, self.cursorY, self.currentColor)
end

function DrawArea:mousemoved(x, y)
    if x < self.x or x > self.x + self.width - 1 or y < self.y or y > self.y + self.height - 1 then
        self.cursorPos = nil
        return
    end

    self.cursorPos = self:getCursorPos(x, y)
    self.currentTool:onMouseMove(self, x, y)
end

function DrawArea:mousepressed(x, y, button)
    if x < self.x or x > self.x + self.width or y < self.y or y > self.y + self.height or button ~= 1 then
        return
    end

    self.currentTool:onMouseDown(self, x, y)
end

function DrawArea:mousereleased(x, y, button)
    self.currentTool:onMouseUp(self, x, y)
end

function DrawArea:getSpriteSize()
    return 8 * self.zoom
end

function DrawArea:getCursorPos(x, y)
    local row = flr((y - self.y) / self.zoomCoef)
    local col = flr((x - self.x) / self.zoomCoef)

    return {
        x = col,
        y = row
    }
end

function DrawArea:getPixel(x, y)
    local cursor = self:getCursorPos(x, y)

    return self:getPixelLocal(cursor.x, cursor.y)
end

function DrawArea:getPixelLocal(x, y)
    local sprX = self.currentSprite % 32 * 8
    local sprY = flr(self.currentSprite / 32) * 8

    return sget(sprX + x, sprY + y)
end

function DrawArea:setPixel(x, y, color)
    local cursor = self:getCursorPos(x, y)

    if color == nil then
        color = self.currentColor
    end

    self:setPixelLocal(cursor.x, cursor.y, color)
end

function DrawArea:setPixelLocal(x, y, color)
    local sprX = self.currentSprite % 32 * 8
    local sprY = flr(self.currentSprite / 32) * 8

    if color == nil then
        color = self.currentColor
    end

    sset(sprX + x, sprY + y, color)
end

return DrawArea
