local Tool = require("os/apps/sprite_editor/tools/Tool")
local SetPixelsCommand = require(
                             "os/apps/sprite_editor/commands/SetPixelsCommand")

local EllipseTool = setmetatable({}, Tool)
EllipseTool.__index = EllipseTool

function EllipseTool.new()
    local self = setmetatable(Tool.new(), EllipseTool)
    self.drawing = false
    self.oldPixels = {}
    self.newPixels = {}
    self.command = nil
    return self
end

function EllipseTool:onMouseDown(drawArea, x, y)
    self:initPixels(drawArea)
    self.command = SetPixelsCommand.new(drawArea, self.oldPixels)
    self.drawing = true
    self.startX = x
    self.startY = y
end

function EllipseTool:onMouseUp(drawArea, x, y)
    if self.drawing then
        self:drawEllipse(drawArea, self.startX, self.startY, x, y)
        if self:isDirty() then
            self.command:setNewPixels(self.newPixels)
            drawArea.commandStack:push(self.command)
        end
        self.newPixels = {}
        self.drawing = false
    end
end

function EllipseTool:onMouseMove(drawArea, x, y)
    if self.drawing then
        self:resetPixels(drawArea)
        self.command:undo()
        self:drawEllipse(drawArea, self.startX, self.startY, x, y)
    end
end

function EllipseTool:drawEllipse(drawArea, x1, y1, x2, y2)
    local cursor1 = drawArea:getCursorPos(x1, y1)
    local cursor2 = drawArea:getCursorPos(x2, y2)
    self:drawEllipseLocal(drawArea, cursor1.x, cursor1.y, cursor2.x, cursor2.y)
end

function EllipseTool:drawEllipseLocal(drawArea, x1, y1, x2, y2)
    -- Calculate ellipse radiuses and center
    local rx = math.abs(x2 - x1) / 2
    local ry = math.abs(y2 - y1) / 2
    local xc = (x1 + x2) / 2
    local yc = (y1 + y2) / 2

    local dx, dy, d1, d2, x, y
    x = 0
    y = ry

    -- Decision parameter of region 1
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx)
    dx = 2 * ry * ry * x
    dy = 2 * rx * rx * y

    -- For region 1
    while (dx < dy) do
        -- Print points based on 4-way symmetry
        self:setLocalPixel(drawArea, xc + x, yc + y)
        self:setLocalPixel(drawArea, xc - x, yc + y)
        self:setLocalPixel(drawArea, xc + x, yc - y)
        self:setLocalPixel(drawArea, xc - x, yc - y)

        -- Checking and updating value of decision parameter based on algorithm
        if (d1 < 0) then
            x = x + 1
            dx = dx + (2 * ry * ry)
            d1 = d1 + dx + (ry * ry)
        else
            x = x + 1
            y = y - 1
            dx = dx + (2 * ry * ry)
            dy = dy - (2 * rx * rx)
            d1 = d1 + dx - dy + (ry * ry)
        end
    end

    -- Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + ((rx * rx) * ((y - 1) * (y - 1))) - (rx * rx * ry * ry)

    -- For region 2
    while (y >= 0) do
        -- Print points based on 4-way symmetry
        self:setLocalPixel(drawArea, xc + x, yc + y)
        self:setLocalPixel(drawArea, xc - x, yc + y)
        self:setLocalPixel(drawArea, xc + x, yc - y)
        self:setLocalPixel(drawArea, xc - x, yc - y)

        -- Checking and updating value of decision parameter based on algorithm
        if (d2 > 0) then
            y = y - 1
            dy = dy - (2 * rx * rx)
            d2 = d2 + (rx * rx) - dy
        else
            y = y - 1
            x = x + 1
            dx = dx + (2 * ry * ry)
            dy = dy - (2 * rx * rx)
            d2 = d2 + dx - dy + (rx * rx)
        end
    end
end

return EllipseTool
