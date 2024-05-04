-- NumbericStepper
local UIElement = require("ui/UIElement")
local NumbericStepper = setmetatable({}, UIElement)
NumbericStepper.__index = NumbericStepper

function NumbericStepper.new(x, y, minVaue, maxValue, changeCallback,
                             iconDrawFunction)
    local self = setmetatable(UIElement.new(x, y, 21, 7), NumbericStepper)
    self.minVaue = minVaue
    self.maxValue = maxValue
    self.value = minVaue
    self.changeCallback = changeCallback
    self.iconDrawFunction = iconDrawFunction
    return self
end

function NumbericStepper:draw()
    local xOffset = 8
    self.iconDrawFunction(self.x, self.y, self.value)

    -- borders
    line(self.x + xOffset, self.y + 1, self.x + xOffset, self.y + 5, 0)
    line(self.x + xOffset + 1, self.y, self.x + xOffset + 11, self.y, 0)
    line(self.x + xOffset + 12, self.y + 1, self.x + xOffset + 12, self.y + 5, 0)
    line(self.x + xOffset + 1, self.y + 6, self.x + xOffset + 11, self.y + 6, 0)

    -- draw - button
    if self.value > self.minVaue then
        rectfill(self.x + xOffset + 1, self.y + 1, 5, 5, 2)
        print("-", self.x + xOffset + 2, self.y + 1, 0)
    else
        rectfill(self.x + xOffset + 1, self.y + 1, 5, 5, 0)
        print("-", self.x + xOffset + 2, self.y + 1, 3)
    end

    -- separator line
    line(self.x + xOffset + 6, self.y + 1, self.x + xOffset + 6, self.y + 5, 0)

    -- draw + button
    if self.value < self.maxValue then
        rectfill(self.x + xOffset + 7, self.y + 1, 5, 5, 2)
        print("+", self.x + xOffset + 8, self.y + 1, 0)
    else
        rectfill(self.x + xOffset + 7, self.y + 1, 5, 5, 0)
        print("+", self.x + xOffset + 8, self.y + 1, 3)
    end
end

function NumbericStepper:increase()
    trace("increase")
    if self.value < self.maxValue then
        self.value = self.value + 1
        self.changeCallback(self.value)
    end
end

function NumbericStepper:decrease()
    trace("decrease")
    if self.value > self.minVaue then
        self.value = self.value - 1
        self.changeCallback(self.value)
    end
end

function NumbericStepper:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and
        y <= self.y + self.height then
        self.isPressed = true
        -- check if - button was pressed
        if x >= self.x + 8 and x <= self.x + 13 and y >= self.y + 1 and y <=
            self.y + 5 then self:decrease() end

        -- check if + button was pressed
        if x >= self.x + 14 and x <= self.x + 20 and y >= self.y + 1 and y <=
            self.y + 5 then self:increase() end
    end
end

function NumbericStepper:mousereleased(x, y, button)
    if button == 1 and self.isPressed then trace("Released!") end
end

return NumbericStepper
