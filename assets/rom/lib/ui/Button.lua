-- Button
local UIElement = require("ui/UIElement")
local Button = setmetatable({}, UIElement)
Button.__index = Button

function Button.new(x, y, width, height, text, callback)
    local self = setmetatable(UIElement.new(x, y, width, height), Button)
    self.text = text or ""
    self.callback = callback or function() end
    self.isPressed = false
    return self
end

function Button:draw()
    rect(self.x, self.y, self.width, self.height, 2)
    rectfill(self.x + 1, self.y + 1, self.width - 2, self.height - 2, 1)
    print(self.text, self.x + self.width / 2 - #self.text*2, self.y + self.height / 2 - 2, 3)
end

function Button:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
        self.isPressed = true
    end
end

function Button:mousereleased(x, y, button)
    if button == 1 and self.isPressed then
        if x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
            self.callback()
        end
        self.isPressed = false
    end
end

return Button