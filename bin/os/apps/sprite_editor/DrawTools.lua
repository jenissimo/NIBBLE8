-- DrawTools
local UIElement = require("ui/UIElement")
local DrawTools = setmetatable({}, UIElement)
DrawTools.__index = DrawTools

local Tool = require("os/apps/sprite_editor/tools/Tool")
local PenTool = require("os/apps/sprite_editor/tools/PenTool")
local RectTool = require("os/apps/sprite_editor/tools/RectTool")
local CircTool = require("os/apps/sprite_editor/tools/CircTool")
local FillTool = require("os/apps/sprite_editor/tools/FillTool")
local LineTool = require("os/apps/sprite_editor/tools/LineTool")

function DrawTools.new(x, y, toolChangeCallback)
    local self = setmetatable(UIElement.new(x, y, 64, 8), DrawTools)
    self.selectedTool = 1
    self.toolChangeCallback = toolChangeCallback
    self.tools = {{
        name = "pencil",
        drawFunction = DrawTools.drawPencil,
        tool = PenTool.new()
    }, {
        name = "stamp",
        drawFunction = DrawTools.drawStamp,
        tool = Tool.new()
    }, {
        name = "select",
        drawFunction = DrawTools.drawSelect,
        tool = Tool.new()
    }, {
        name = "fill",
        drawFunction = DrawTools.drawFill,
        tool = FillTool.new()
    }, {
        name = "circle",
        drawFunction = DrawTools.drawCircle,
        tool = CircTool.new()
    }, {
        name = "rect",
        drawFunction = DrawTools.drawRect,
        tool = RectTool.new()
    }, {
        name = "line",
        drawFunction = DrawTools.drawLine,
        tool = LineTool.new()
    }}
    return self
end

function DrawTools:draw()
    -- draw tools
    for i = 1, #self.tools do
        if i == self.selectedTool then
            self.tools[i].drawFunction(self.x + (i - 1) * 8, self.y, 3)
        else
            self.tools[i].drawFunction(self.x + (i - 1) * 8, self.y, 2)
        end
    end
end

function DrawTools:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
        self.isPressed = true
        for i = 1, #self.tools do
            local rectX = self.x + (i - 1) * 8
            local rectY = self.y
            local rectW = 8
            local rectH = 8
            if x >= rectX and x <= rectX + rectW and y >= rectY and y <= rectY + rectH then
                self.selectedTool = i
                self.toolChangeCallback(self.tools[i].tool)
                trace("Selected tool: " .. self.tools[i].name)
            end
            self.isPressed = false
        end
    end
end

function DrawTools:mousereleased(x, y, button)
    if button == 1 and self.isPressed then
        trace("Released!")
    end
end

function DrawTools:setSelectedTool(toolIndex)
    if toolIndex < 1 or toolIndex > #self.tools then
        return
    end
    
    self.selectedTool = toolIndex
    self.toolChangeCallback(self.tools[toolIndex].tool)
end

function DrawTools.drawPencil(x, y, col)
    -- pencil shadow
    line(x, y + 5, x + 4, y + 1, 0)
    line(x + 1, y + 6, x + 5, y + 2, 0)
    pset(x, y + 6, 0)
    -- pencil
    line(x, y + 4, x + 4, y, col)
    line(x + 1, y + 5, x + 5, y + 1, col)
    pset(x, y + 5, col)
end

function DrawTools.drawStamp(x, y, col)
    -- stamp shadow
    rectfill(x + 2, y + 1, 2, 3, 0)
    rect(x, y + 4, 6, 3, 0)
    -- stamp
    rectfill(x + 2, y, 2, 3, col)
    rect(x, y + 3, 6, 3, col)
end

function DrawTools.drawSelect(x, y, col)
    -- select shadow
    -- left top corner
    line(x, y, x + 1, y + 1, 0)
    line(x, y, x, y + 2, 0)

    -- right top corner
    line(x + 5, y, x + 4, y + 1, 0)
    line(x + 5, y, x + 5, y + 2, 0)

    -- left bottom corner
    line(x, y + 6, x, y + 5, 0)
    line(x, y + 6, x + 1, y + 6, 0)

    -- right bottom corner
    line(x + 5, y + 6, x + 5, y + 5, 0)
    line(x + 5, y + 6, x + 4, y + 6, 0)

    -- select
    -- left top corner
    line(x, y, x + 1, y, col)
    line(x, y, x, y + 1, col)

    -- right top corner
    line(x + 5, y, x + 4, y, col)
    line(x + 5, y, x + 5, y + 1, col)

    -- left bottom corner
    line(x, y + 5, x, y + 4, col)
    line(x, y + 5, x + 1, y + 5, col)

    -- right bottom corner
    line(x + 5, y + 5, x + 5, y + 4, col)
    line(x + 5, y + 5, x + 4, y + 5, col)
end

function DrawTools.drawFill(x, y, col)
    -- fill shadow
    line(x + 3, y + 1, x + 5, y + 3, 0)
    line(x, y + 5, x, y + 6, 0)
    line(x + 1, y + 4, x + 5, y + 4, 0)
    line(x + 2, y + 5, x + 4, y + 5, 0)
    line(x + 3, y + 6, x + 3, y + 6, 0)

    -- fill
    line(x + 3, y, x + 5, y + 2, col)
    line(x, y + 4, x, y + 5, col)
    line(x + 1, y + 3, x + 5, y + 3, col)
    line(x + 2, y + 4, x + 4, y + 4, col)
    line(x + 3, y + 5, x + 3, y + 5, col)
end

function DrawTools.drawCircle(x, y, col)
    -- circle shadow
    line(x, y + 2, x, y + 5, 0)
    line(x + 1, y + 1, x + 4, y + 1, 0)
    line(x + 5, y + 2, x + 5, y + 5, 0)
    line(x + 1, y + 6, x + 4, y + 6, 0)
    -- circle
    line(x, y + 1, x, y + 4, col)
    line(x + 1, y, x + 4, y, col)
    line(x + 5, y + 1, x + 5, y + 4, col)
    line(x + 1, y + 5, x + 4, y + 5, col)
end

function DrawTools.drawRect(x, y, col)
    -- rect
    rect(x, y + 1, 6, 6, 0)
    rect(x, y, 6, 6, col)
end

function DrawTools.drawLine(x, y, col)
    -- line
    line(x, y + 4, x + 3, y + 1, col)
    line(x, y + 5, x + 3, y + 2, 0)
    -- line(x, y + 5, x + 3, y, 0)
end

return DrawTools
