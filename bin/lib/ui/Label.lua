-- Label
local UIElement = require("ui/UIElement")
local Label = setmetatable({}, UIElement)
Label.__index = Label

function Label.new(x, y, text, color)
    local width = 4 * #text
    local height = 5
    local self = setmetatable(UIElement.new(x, y, width, height), Label)
    self.text = text or ""
    self.color = color or 3
    return self
end

function Label:setText(text)
    self.text = text
end

function Label:draw()
    print(self.text, self.x, self.y, self.color)
    --love.graphics.setColor(1, 1, 1, 1)
    --love.graphics.printf(self.text, self.x, self.y + self.height / 2 - 10, self.width, "center")
end

return Label