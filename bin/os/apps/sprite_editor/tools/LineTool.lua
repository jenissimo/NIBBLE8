local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require("os/apps/sprite_editor/commands/SetPixelsCommand")

local LineTool = setmetatable({}, Tool)
LineTool.__index = LineTool

function LineTool.new()
    local self = setmetatable(Tool.new(), LineTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    return self
end

function LineTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self.startX = x
    self.startY = y
end

function LineTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        self:drawLine(drawArea, self.startX, self.startY, x, y)
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function LineTool:onMouseMove(drawArea, x, y)
    if self.drawing then
        self.command:undo()
        self:resetPixels(drawArea)
        self:drawLine(drawArea, self.startX, self.startY, x, y)
    end
end

function LineTool:drawLine(drawArea, x1, y1, x2, y2)
    local dx, dy = math.abs(x2 - x1), math.abs(y2 - y1)
    local sx, sy = x1 < x2 and 1 or -1, y1 < y2 and 1 or -1
    local err = dx - dy
    local sprSize = drawArea:getSpriteSize()

    while true do
        self:setPixel(drawArea, x1, y1)
        
        if x1 == x2 and y1 == y2 then
            break
        end
        local e2 = 2 * err
        if e2 > -dy then
            err = err - dy
            x1 = x1 + sx
        end
        if e2 < dx then
            err = err + dx
            y1 = y1 + sy
        end
    end
end

return LineTool