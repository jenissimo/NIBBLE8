-- Knob
local UIElement = require("ui/UIElement")
local HGroup = setmetatable({}, UIElement)
HGroup.__index = HGroup

function HGroup.new(x, y, label, hSpacing, padding, uiManager)
    local self = setmetatable(UIElement.new(x, y, 0, 0), HGroup)
    self.uiManager = uiManager
    self.label = label
    self.hSpacing = hSpacing
    self.padding = padding or {0,0,0,0} -- left, top, right, bottom
    self.elements = {}
    return self
end

function HGroup:addElement(element)
    table.insert(self.elements, element)
    --self.uiManager:addElement(element)
    self:updateElements()
end

function HGroup:updateElements()
    local width = 0
    local height = 0
    local totalWidth = 0
    local totalSpacing = self.hSpacing * (#self.elements - 1)

    local index = 0

    for i = 1, #self.elements do
        local element = self.elements[i]
        totalWidth = totalWidth + element.width
        if (element.height > height) then
            height = element.height
        end
    end

    local posX = self.x + self.padding[1]
    for i = 1, #self.elements do
        local element = self.elements[i]
        element.x = posX
        element.y = self.y + self.padding[2]
        posX = posX + element.width + self.hSpacing
    end

    width = totalWidth + totalSpacing + self.padding[1] + self.padding[3]
    height = height + self.padding[2] + self.padding[4]

    self.width = width
    self.height = height
end

function HGroup:update()
    for i = 1, #self.elements do
        local element = self.elements[i]
        element:update()
    end
end

function HGroup:draw()
    -- draw lines
    rect(self.x, self.y, self.width, self.height, 3)

    local prx = self.x + (self.width - (#self.label * 4 - 1)) / 2 + 1
    local pry = self.y - 2
    local textPadding = 4

    rectfill(prx - textPadding, pry - 1, #self.label * 4 + textPadding * 2 - 1,
             7, 0)
    print(self.label, prx, pry, 3)

    for i = 1, #self.elements do
        local element = self.elements[i]
        element:draw()
    end
end

function HGroup:mousepressed(x, y, button)
    for i = 1, #self.elements do
        local element = self.elements[i]
        element:mousepressed(x, y, button)
    end
end

function HGroup:mousemoved(x, y)
    for i = 1, #self.elements do
        local element = self.elements[i]
        element:mousemoved(x, y)
    end
end

function HGroup:mousereleased(x, y, button)
    for i = 1, #self.elements do
        local element = self.elements[i]
        element:mousereleased(x, y, button)
    end
end

return HGroup
