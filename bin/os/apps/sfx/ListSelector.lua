-- ListSelector
local UIElement = require("ui/UIElement")
local UIUtils = require("ui/UIUtils")
local ArrowButton = require("ui/ArrowButton")
local ListSelector = setmetatable({}, UIElement)
ListSelector.__index = ListSelector

function ListSelector.new(x, y, label,dataProvider, maxChars, changeCallback)
    local self = setmetatable(UIElement.new(x, y, 23, 9), ListSelector)
    self.label = label
    self.dataProvider = dataProvider
    self.valueWidth = maxChars * 4 - 1
    self.hSpacing = 1
    self.selectedIndex = 1
    self.changeCallback = changeCallback
    self.upButton = ArrowButton.new(x + 14, y, "up", 2, 0,
                                    function() self:increase() end)
    self.downButton = ArrowButton.new(x + 8, y, "down", 2, 0,
                                      function() self:decrease() end)
    return self
end

function ListSelector:draw()
    local hAdvance = self.x

    -- label
    UIUtils.fancyPrint(self.label, hAdvance, self.y + 2, 3, 0)
    hAdvance = hAdvance + (#self.label * 4) + self.hSpacing - 1

    -- value
    UIUtils.fancyRect(hAdvance, self.y, self.valueWidth + 2, 8, 2, 3, 0)
    print(self.dataProvider[self.selectedIndex], hAdvance + 2, self.y + 2, 3)
    hAdvance = hAdvance + self.valueWidth + 2 + self.hSpacing + 1

    -- buttons
    self.upButton.x = hAdvance
    self.upButton.y = self.y + 3
    self.downButton.x = hAdvance
    self.downButton.y = self.y + 5

    self.upButton:draw()
    self.downButton:draw()
end

function ListSelector:increase()
    --trace("increase")
    if self.selectedIndex < #self.dataProvider then
        self.selectedIndex = self.selectedIndex + 1
        self.changeCallback(self.selectedIndex)
    end
end

function ListSelector:decrease()
    --trace("decrease")
    if self.selectedIndex > 0 then
        self.selectedIndex = self.selectedIndex - 1
        self.changeCallback(self.selectedIndex)
    end
end

function ListSelector:mousepressed(x, y, button)
    self.upButton:mousepressed(x, y, button)
    self.downButton:mousepressed(x, y, button)
end

function ListSelector:mousereleased(x, y, button)
    self.upButton:mousereleased(x, y, button)
    self.downButton:mousereleased(x, y, button)
end

return ListSelector
