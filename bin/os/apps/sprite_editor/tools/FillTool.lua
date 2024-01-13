local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require(
                             "os/apps/sprite_editor/commands/SetPixelsCommand")

local FillTool = setmetatable({}, Tool)
FillTool.__index = FillTool

function FillTool.new()
    local self = setmetatable(Tool.new(), FillTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    return self
end

function FillTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self.startX = x
    self.startY = y
end

function FillTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        local cursor = drawArea:getCursorPos(x, y)
        self:fill(drawArea, cursor.x, cursor.y,
                  drawArea:getPixelLocal(cursor.x, cursor.y))
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function FillTool:onMouseMove(drawArea, x, y)
    -- Do nothing
end

function FillTool:fill(drawArea, x, y, targetColor)
    if not self:isInBounds(drawArea, x, y) then return end

    if self.newPixels[y + 1][x + 1] == targetColor then
        self:setLocalPixel(drawArea, x, y)

        -- Recursive calls for adjacent pixels
        self:fill(drawArea, x + 1, y, targetColor)
        self:fill(drawArea, x - 1, y, targetColor)
        self:fill(drawArea, x, y + 1, targetColor)
        self:fill(drawArea, x, y - 1, targetColor)
    end
end

return FillTool
