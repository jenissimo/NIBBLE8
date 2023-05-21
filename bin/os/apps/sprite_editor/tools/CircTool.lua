local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require("os/apps/sprite_editor/commands/SetPixelsCommand")

local CircTool = setmetatable({}, Tool)
CircTool.__index = CircTool

function CircTool.new()
    local self = setmetatable(Tool.new(), CircTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    return self
end

function CircTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self.startX = x
    self.startY = y
end

function CircTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        self:drawCirc(drawArea, self.startX, self.startY, x, y)
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function CircTool:onMouseMove(drawArea, x, y)
    if self.drawing then
        self:resetPixels(drawArea)
        self.command:undo()
        self:drawCirc(drawArea, self.startX, self.startY, x, y)
    end
end

function CircTool:drawCirc(drawArea, x1, y1, x2, y2)
    -- Calculate center and radius based on bounding box
    local cx = flr((x1 + x2) / 2)
    local cy = flr((y1 + y2) / 2)
    local radius = flr(math.min(math.abs(x2 - x1), math.abs(y2 - y1)) / 2)

    -- Ensure radius is at least 1 to avoid drawing nothing
    if radius < 1 then
        radius = 1
    end

    -- Midpoint circle algorithm
    local x = radius
    local y = 0
    local err = 0

    while x >= y do
        self:setPixel(drawArea, cx + x, cy + y)
        self:setPixel(drawArea, cx + y, cy + x)
        self:setPixel(drawArea, cx - y, cy + x)
        self:setPixel(drawArea, cx - x, cy + y)
        self:setPixel(drawArea, cx - x, cy - y)
        self:setPixel(drawArea, cx - y, cy - x)
        self:setPixel(drawArea, cx + y, cy - x)
        self:setPixel(drawArea, cx + x, cy - y)

        if err <= 0 then
            y = y + 1
            err = err + 2 * y + 1
        end

        if err > 0 then
            x = x - 1
            err = err - 2 * x + 1
        end
    end
end

return CircTool
