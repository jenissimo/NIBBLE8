-- ColorPicker
local UIElement = require("ui/UIElement")
local ColorPicker = setmetatable({}, UIElement)
ColorPicker.__index = ColorPicker

local function drawArrow(x, y, col)
    for i=0,2 do
        line(x+i,y+i,x+4-i,y+i,col)
        line(x+i,y+i+1,x+4-i,y+i+1,0)
    end
end

function ColorPicker.new(x, y)
    local self = setmetatable(UIElement.new(x, y, 28, 7), ColorPicker)
    self.selectedColor = 0
    return self
end

function ColorPicker:draw()
    rect(self.x-1,self.y-1,self.width+2,self.height+2,0)
    -- top border
    line(
        self.x-1,
        self.y-2,
        self.x + self.width,
        self.y-2,
        2
    )

    -- left border
    line(
        self.x-2,
        self.y-1,
        self.x-2,
        self.y+self.height,
        2
    )

    -- right border
    line(
        self.x + self.width + 1,
        self.y - 1,
        self.x + self.width + 1,
        self.y+self.height,
        3
    )

    -- bottom border
    line(
        self.x-1,
        self.y + self.height + 1,
        self.x + self.width,
        self.y + self.height + 1,
        3
    )

    for i=0,3 do
        rectfill(self.x+i*7,self.y,7,7,i)
        print(str(i), self.x + i*7 + 2, self.y + 1, 3-i)
    end

    drawArrow(self.x+self.selectedColor*7+1,self.y-7,3)
end

function ColorPicker:mousepressed(x, y, button)
    if button == 1 and x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y + self.height then
        self.isPressed = true
    end
end

function ColorPicker:mousereleased(x, y, button)
    if button == 1 and self.isPressed then
        trace("Released!")
        for i=0,3 do
            local rectX=self.x+i*7
            local rectY=self.y
            local rectW = 7
            local rectH = 7
            if x >= rectX and x <= rectX + rectW and y >= rectY and y <= rectY + rectH then
                self.selectedColor = i
            end
            self.isPressed = false
        end
        
    end
end

return ColorPicker