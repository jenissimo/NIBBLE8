-- SpriteFlagsSelector
local UIElement = require("ui/UIElement")
local SpriteFlagsSelector = setmetatable({}, UIElement)
SpriteFlagsSelector.__index = SpriteFlagsSelector

function SpriteFlagsSelector.new(x, y)
    local self = setmetatable(UIElement.new(x, y, 47, 5), SpriteFlagsSelector)
    self.selectedSprite = 0
    self.flags = {false, false, false, false, false, false, false, false}
    return self
end

function SpriteFlagsSelector:drawButton(x, y, selected) 
    local radius = 2

    if selected then
        circfill(x + 3, y + 3, radius, 2)
        circ(x + 3, y + 3, radius, 0)
        pset(x + radius * 2, y + 2, 3)
    else
        circfill(x + 3, y + 3, radius, 0)
    end
end

function SpriteFlagsSelector:draw()
    for i=0,7 do
        self:drawButton(self.x + i*6, self.y, self.flags[i+1])
    end
end

function SpriteFlagsSelector:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and
        y <= self.y + self.height then self.isPressed = true
        self.flags[math.floor((x - self.x) / 6) + 1] = not self.flags[math.floor((x - self.x) / 6) + 1]
    end
end

function SpriteFlagsSelector:mousereleased(x, y, button)
    if button == 1 and self.isPressed then trace("Released!") end
end

return SpriteFlagsSelector
