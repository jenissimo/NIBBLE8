-- ArrowButton
local UIElement = require("ui/UIElement")
local ArrowButton = setmetatable({}, UIElement)
ArrowButton.__index = ArrowButton

function ArrowButton.new(x, y, direction, col, shadowCol, callback)
    local dir = direction or "up"
    local width
    local height

    if dir == "up" or dir == "down" then
        width = 5
        height = 3
    elseif dir == "left" or dir == "right" then
        width = 3
        height = 5
    end

    local self = setmetatable(UIElement.new(x, y, width, height), ArrowButton)
    self.direction = dir
    self.col = col or 3
    self.shadowCol = shadowCol
    self.callback = callback or function() end
    self.isPressed = false
    self.enabled = true

    return self
end

function ArrowButton:draw()
    local x = self.x
    local y = self.y
    local direction = self.direction
    local x1, y1, x2, y2

    if not self.enabled then
        col = 2
        shadowCol = 1
    end

    if direction == "down" then
        for i = 0, 2 do
            x1 = x + i
            y1 = y + i
            x2 = x + 4 - i
            y2 = y + i
            line(x1,y1,x2,y2, self.col)
            if self.shadowCol then
                line(x1,y1+1,x2,y2+1, self.shadowCol)
            end
        end
    elseif direction == "up" then
        for i = 0, 2 do
            x1 = x + i
            y1 = y - i
            x2 = x + 4 - i
            y2 = y - i
            line(x1,y1,x2,y2, self.col)
        end
        if self.shadowCol then
            line(x,y+1,x2+2,y+1, self.shadowCol)
        end
    elseif direction == "left" then
        for i = 0, 2 do
            x1 = x - i
            y1 = y + i
            x2 = x - i
            y2 = y + 4 - i

            if self.shadowCol then
                line(x1,y1+1,x2,y2+1, self.shadowCol)
            end
            line(x1,y1,x2,y2, self.col)
        end
    elseif direction == "right" then
        for i = 0, 2 do
            x1 = x + i
            y1 = y + i
            x2 = x + i
            y2 = y + 4 - i

            if self.shadowCol then
                line(x1,y1+1,x2,y2+1, self.shadowCol)
            end
            line(x1,y1,x2,y2, self.col)
        end
    end
end

function ArrowButton:drawUp()
    
end

function ArrowButton:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and
        y <= self.y + self.height then self.isPressed = true end
end

function ArrowButton:mousereleased(x, y, button)
    if button == 1 and self.isPressed then
        if x >= self.x and x <= self.x + self.width and y >= self.y and y <=
            self.y + self.height then self.callback() end
        self.isPressed = false
    end
end

return ArrowButton
