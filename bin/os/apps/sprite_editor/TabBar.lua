-- ColorPicker
local UIElement = require("ui/UIElement")
local TabBar = setmetatable({}, UIElement)
TabBar.__index = TabBar

function TabBar:drawTab(x, y, col, index)
    line(x,y+1,x,y+5,col)
    rectfill(x+1,y,5,7,col)
    print(str(index), x+2, y+1, 3-col)
end

function TabBar.new(x, y, tabsCount)
    local self = setmetatable(UIElement.new(x, y, 6, 8*tabsCount), TabBar)
    self.selectedTab = 0
    self.tabsCount = tabsCount
    return self
end

function TabBar:draw()
    for i=0,self.tabsCount-1 do
        local col = 2
        if i == self.selectedTab then
            col = 3
        end

        self:drawTab(self.x,self.y+i*7 + i, col, i)
    end
end

function TabBar:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
        trace("Pressed!")
        self.isPressed = true
    end
end

function TabBar:mousereleased(x, y, button)
    if button == 1 and self.isPressed and x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
        trace("Released!")

        local tab = math.floor((y - self.y) / 8)
        if tab >= 0 and tab <= self.tabsCount then
            self.selectedTab = tab
        end

        isPressed = false
    end
end

return TabBar