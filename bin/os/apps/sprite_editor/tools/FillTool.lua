local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require("os/apps/sprite_editor/commands/SetPixelsCommand")

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
        self:fill(drawArea, x, y)
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

function FillTool:fill(drawArea, x, y)
    local cursor = drawArea:getCursorPos(x, y)
    local oldColor = drawArea:getPixel(x, y)
    if oldColor == newColor then
        return
    end
    self.newPixels[cursor.y + 1] = {}
    self:fillPixel(drawArea, cursor.x, cursor.y, oldColor)
end

function FillTool:fillPixel(drawArea, x, y, oldColor)
    local cursor = drawArea:getCursorPos(x, y)
    if not self:isCursorInBounds(drawArea, cursor) or drawArea:getPixel(x, y) ~= oldColor then
        return
    end
    self:setPixel(drawArea, x, y)

    self:fillPixel(drawArea, x + 1, y, oldColor)
    self:fillPixel(drawArea, x - 1, y, oldColor)
    self:fillPixel(drawArea, x, y + 1, oldColor)
    self:fillPixel(drawArea, x, y - 1, oldColor)
end

return FillTool