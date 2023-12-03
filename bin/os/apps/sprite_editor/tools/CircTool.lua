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
    local cursor1 = drawArea:getCursorPos(x1, y1)
    local cursor2 = drawArea:getCursorPos(x2, y2)
    self:drawCircLocal(drawArea, cursor1.x, cursor1.y, cursor2.x, cursor2.y)
end

function CircTool:drawCircLocal(drawArea, x1, y1, x2, y2)
    local width = x2 - x1
    local height = y2 - y1
    local centerX = x1 + width / 2
    local centerY = y1 + height / 2
    local widthRadius = width / 2
    local heightRadius = height / 2

    for y = y1, y2 do
        for x = x1, x2 do
            local dx = (x - centerX) / widthRadius
            local dy = (y - centerY) / heightRadius
            if dx * dx + dy * dy <= 1 then
                self:setLocalPixel(drawArea, x, y)
            end
        end
    end
end

function CircTool:drawCircLocal2(drawArea, x1, y1, x2, y2)
    local dx, dy = x2 - x1, y2 - y1
    local radius = flr(math.sqrt(dx * dx + dy * dy))
    local x, y = radius, 0
    local decisionOver2 = 1 - x;

    while y <= x do
        self:setLocalPixel(drawArea, x1 + radius + x, y1 + radius + y);
        self:setLocalPixel(drawArea, x1 + radius + y, y1 + radius + x);
        self:setLocalPixel(drawArea, x1 + radius - x, y1 + radius + y);
        self:setLocalPixel(drawArea, x1 + radius - y, y1 + radius + x);

        self:setLocalPixel(drawArea, x1 + radius - x, y1 + radius - y);
        self:setLocalPixel(drawArea, x1 + radius - y, y1 + radius - x);
        self:setLocalPixel(drawArea, x1 + radius + x, y1 + radius - y);
        self:setLocalPixel(drawArea, x1 + radius + y, y1 + radius - x);

        y = y + 1;
        if decisionOver2 < 0 then
            decisionOver2 = decisionOver2 + 2 * y + 1;
        else
            x = x - 1;
            decisionOver2 = decisionOver2 + 2 * (y - x) + 1;
        end
    end
end

return CircTool
