local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require("os/apps/sprite_editor/commands/SetPixelsCommand")

local PenTool = setmetatable({}, Tool)
PenTool.__index = PenTool

function PenTool.new()
    local self = setmetatable(Tool.new(), PenTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    return self
end

function PenTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self:setPixel(drawArea, x, y)
end

function PenTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function PenTool:onMouseMove(drawArea, x, y)
    if self.drawing then
        self:setPixel(drawArea, x, y)
    end
end

return PenTool
