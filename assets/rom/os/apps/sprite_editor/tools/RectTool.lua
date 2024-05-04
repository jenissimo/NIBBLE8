local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require("os/apps/sprite_editor/commands/SetPixelsCommand")

local RectTool = setmetatable({}, Tool)
RectTool.__index = RectTool

function RectTool.new()
    local self = setmetatable(Tool.new(), RectTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    self.lastCursor = nil
    return self
end

function RectTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self.startX = x
    self.startY = y
end

function RectTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        self:drawRect(drawArea, self.startX, self.startY, x, y)
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function RectTool:onMouseMove(drawArea, x, y)
    if self.drawing then
        local newCursorPos = drawArea:getCursorPos(x, y)
        -- check if the cursor has moved
        if self.lastCursor and self.lastCursor.x == newCursorPos.x and self.lastCursor.y == newCursorPos.y then
            return
        end
        self.lastCursor = newCursorPos

        self:resetPixels(drawArea)
        self.command:undo()
        self:drawRect(drawArea, self.startX, self.startY, x, y)
    end
end

function RectTool:drawRect(drawArea, x1, y1, x2, y2)
    local startX, endX = math.min(x1, x2), math.max(x1, x2)
    local startY, endY = math.min(y1, y2), math.max(y1, y2)
    local sprSize = drawArea:getSpriteSize()

    for x = startX, endX do
        self:setPixel(drawArea, x, startY)
        self:setPixel(drawArea, x, endY)
    end
    for y = startY, endY do
        self:setPixel(drawArea, startX, y)
        self:setPixel(drawArea, endX, y)
    end
end

return RectTool
