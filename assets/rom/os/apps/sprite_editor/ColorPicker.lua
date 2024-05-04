-- ColorPicker
local UIElement = require("ui/UIElement")
local ColorPicker = setmetatable({}, UIElement)
ColorPicker.__index = ColorPicker

function ColorPicker.new(x, y)
    local self = setmetatable(UIElement.new(x, y, 28, 7), ColorPicker)
    self.selectedColor = 0
    return self
end

function ColorPicker:draw()
    for i = 0, 3 do
        rect(self.x + i * 6, self.y, 7, 7, 0)
        rectfill(self.x + i * 6 + 1, self.y + 1, 5, 5, i)
    end

    rect(self.x + self.selectedColor * 6, self.y, 7, 7, 3)

    -- drawArrow(self.x+self.selectedColor*7+1,self.y-7,3)
end

function ColorPicker:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and
        y <= self.y + self.height then self.isPressed = true end
end

function ColorPicker:mousereleased(x, y, button)
    if button == 1 and self.isPressed then
        for i = 0, 3 do
            local rectX = self.x + i * 7
            local rectY = self.y
            local rectW = 7
            local rectH = 7
            if x >= rectX and x <= rectX + rectW and y >= rectY and y <= rectY +
                rectH then self.selectedColor = i end
            self.isPressed = false
        end
        trace("Selected color: " .. self.selectedColor)

    end
end

return ColorPicker
