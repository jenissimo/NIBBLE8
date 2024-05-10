-- FancyStepper
local UIElement = require("ui/UIElement")
local UIUtils = require("ui/UIUtils")
local ArrowButton = require("ui/ArrowButton")
local FancyStepper = setmetatable({}, UIElement)
FancyStepper.__index = FancyStepper

function FancyStepper.new(x, y, minValue, maxValue, label, changeCallback)
    local self = setmetatable(UIElement.new(x, y, 23, 9), FancyStepper)
    self.minValue = minValue
    self.maxValue = maxValue
    self.hSpacing = 1
    self.digits = #str(maxValue)
    self.label = label
    self.value = minValue
    self.changeCallback = changeCallback
    self.upButton = ArrowButton.new(x + 14, y, "up", 2, 0,
                                    function() self:increase() end)
    self.downButton = ArrowButton.new(x + 8, y, "down", 2, 0,
                                      function() self:decrease() end)
    return self
end

function FancyStepper:draw()
    local hAdvance = self.x

    -- label
    UIUtils.fancyPrint(self.label, hAdvance, self.y + 2, 3, 0)
    hAdvance = hAdvance + (#self.label * 4) + self.hSpacing - 1

    -- value
    UIUtils.fancyRect(hAdvance, self.y, self.digits * 4 + 2, 8, 2, 3, 0)
    print(string.format("%02d", self.value), hAdvance + 2, self.y + 2, 3)
    hAdvance = hAdvance + self.digits * 4 + 2 + self.hSpacing + 1

    -- buttons
    self.upButton.x = hAdvance
    self.upButton.y = self.y + 3
    self.downButton.x = hAdvance
    self.downButton.y = self.y + 5

    self.upButton:draw()
    self.downButton:draw()
end

function FancyStepper:increase()
    if self.value < self.maxValue then
        self.value = self.value + 1
    else
        self.value = self.minValue  -- Wrap around to the minimum value
    end
    self.changeCallback(self.value)
end

function FancyStepper:decrease()
    if self.value > self.minValue then
        self.value = self.value - 1
    else
        self.value = self.maxValue  -- Wrap around to the maximum value
    end
    self.changeCallback(self.value)
end

function FancyStepper:mousepressed(x, y, button)
    self.upButton:mousepressed(x, y, button)
    self.downButton:mousepressed(x, y, button)
end

function FancyStepper:mousereleased(x, y, button)
    self.upButton:mousereleased(x, y, button)
    self.downButton:mousereleased(x, y, button)
end

return FancyStepper
