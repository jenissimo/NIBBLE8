-- WaveSelector
local UIElement = require("ui/UIElement")
local WaveSelector = setmetatable({}, UIElement)
WaveSelector.__index = WaveSelector

function WaveSelector.new(x, y, change_callback)
    local tabsCount = 7
    local tabSpacing = 2

    local self = setmetatable(UIElement.new(x, y, 12, 8 * tabsCount +
                                                (tabsCount - 1) * tabSpacing),
                              WaveSelector)
    self.selectedTab = 1
    self.tabSpacing = tabSpacing
    self.waves = {
        {name = "triangle", draw_fn = self.draw_triangle},
        {name = "tilted saw", draw_fn = self.draw_tilted_saw},
        {name = "saw", draw_fn = self.draw_saw},
        {name = "square", draw_fn = self.draw_square},
        {name = "pulse", draw_fn = self.draw_pulse},
        {name = "noise", draw_fn = self.draw_noise},
        {name = "sine", draw_fn = self.draw_sine}
    }
    self.change_callback = change_callback

    return self
end

function WaveSelector:drawTab(x, y, col, index)
    rect(x, y, self.waves[index].width, self.waves[index].height, col)
    self.waves[index].draw_fn(x + 2, y + 2, col)
end

function WaveSelector:draw()
    for i = 1, #self.waves do
        local col = 1
        if i == self.selectedTab then col = 3 end

        self.waves[i].x = self.x
        self.waves[i].y = self.y + (i - 1) * 8 + (i - 1) * 2
        self.waves[i].width = 12
        self.waves[i].height = 8

        self:drawTab(self.waves[i].x, self.waves[i].y, col, i)
    end
end

function WaveSelector:mousepressed(x, y, button)
    -- check bounds
    if x < self.x or x > self.x + self.width or y < self.y or y > self.y +
        self.height then return end

    for i, item in ipairs(self.waves) do
        if x >= item.x and x <= item.x + item.width and y >= item.y and y <=
            item.y + item.height then
            self.selectedTab = i
            self.change_callback()
        end
    end

    self.isPressed = true
end

function WaveSelector:mousereleased(x, y, button) isPressed = false end

function WaveSelector.draw_triangle(x, y, col)
    line(x, y + 3, x + 3, y, col)
    line(x + 4, y, x + 7, y + 3, col)
end

function WaveSelector.draw_tilted_saw(x, y, col)
    line(x, y + 3, x + 5, y, col)
    line(x + 5, y, x + 7, y + 3, col)
end

function WaveSelector.draw_saw(x, y, col)
    line(x, y + 3, x + 7, y, col)
    line(x + 7, y, x + 7, y + 3, col)
end

function WaveSelector.draw_square(x, y, col)
    -- line right
    line(x, y + 3, x + 3, y + 3, col)
    -- line up
    line(x + 3, y + 3, x + 3, y, col)
    -- line right
    line(x + 3, y, x + 6, y, col)
    -- line down
    line(x + 6, y, x + 6, y + 3, col)
    -- line right
    line(x + 6, y + 3, x + 7, y + 3, col)
end

function WaveSelector.draw_pulse(x, y, col)
    -- line right
    line(x, y + 3, x + 4, y + 3, col)
    -- line up
    line(x + 4, y + 3, x + 4, y, col)
    -- line right
    line(x + 4, y, x + 6, y, col)
    -- line down
    line(x + 6, y, x + 6, y + 3, col)
    -- line right
    line(x + 6, y + 3, x + 7, y + 3, col)
end

function WaveSelector.draw_noise(x, y, col)
    pset(x, y, col)
    pset(x + 2, y, col)
    pset(x, y + 2, col)
    pset(x + 1, y + 3, col)
    pset(x + 3, y + 2, col)
    pset(x + 4, y + 1, col)
    pset(x + 5, y + 3, col)
    pset(x + 6, y, col)
    pset(x + 6, y + 2, col)
    pset(x + 7, y + 1, col)
end

function WaveSelector.draw_sine(x, y, col)
    pset(x, y + 3, col)
    line(x + 1, y + 2, x + 1, y + 1, col)
    line(x + 2, y, x + 3, y, col)
    line(x + 4, y + 2, x + 4, y + 1, col)
    line(x + 5, y + 3, x + 6, y + 3, col)
    line(x + 7, y + 2, x + 7, y + 1, col)
end

return WaveSelector
