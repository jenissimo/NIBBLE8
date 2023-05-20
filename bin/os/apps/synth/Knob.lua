-- Knob
local UIElement = require("ui/UIElement")
local Knob = setmetatable({}, UIElement)
Knob.__index = Knob

function Knob.new(x, y, radius, min_value, max_value, value, drag_range, label, change_callback)
    local self = setmetatable(UIElement.new(x, y, radius*2+1, radius*2+1), Knob)
    self.radius = radius
    self.min_value = min_value
    self.max_value = max_value
    self.value = value
    self.drag_range = drag_range
    self.dragging = false
    self.label = label
    self.change_callback = change_callback
    return self
end

function Knob:draw()
    local cx = self.x + self.radius
    local cy = self.y + self.radius

    circ(cx, cy, self.radius, 2)
    local angle = self:angleFromValue(self.value)
    local ind_x = cx + self.radius * 0.5 * cos(angle)
    local ind_y = cy + self.radius * 0.5 * sin(angle)

    if angle == 0.5 then ind_y = cy end

    line(cx, cy, ind_x, ind_y, 3)

    if self.dragging then
        local strval = str(self.value)
        local prx = self.x + (self.width - #strval * 4) / 2 + 1
        local pry = self.y - 7

        rectfill(prx - 1, pry - 1, #strval * 4 + 1, 7, 1)
        print(strval, prx, pry, 3)
    end

    if self.label then
        -- print centered
        local prx = self.x + (self.width - #self.label * 4) / 2 + 1
        local pry = self.y + self.height + 1
        print(self.label, prx, pry, 3)
    end
end

function Knob:mousepressed(x, y, button)
    -- check bounds
    if x < self.x or x > self.x + self.width or y < self.y or
        y > self.y + self.height then return end
    self.dragging = true
end

function Knob:mousemoved(x, y)
    if self.dragging then self:updateKnobValue(self.x, y) end
end

function Knob:mousereleased(x, y, button)
    if self.dragging then
        self.change_callback()
        self.dragging = false
    end
end

function Knob:updateKnobValue(x, y)
    local cy = self.y + self.radius
    local totalVerticalRange = self.radius * self.drag_range
    local clampedY = mid(cy - totalVerticalRange / 2, y,
                         cy + totalVerticalRange / 2)
    local normalizedY = 1 -
                            ((clampedY - (cy - totalVerticalRange / 2)) /
                                totalVerticalRange)
    local value = mid(self.min_value, normalizedY, self.max_value)
    value = self:round(value, 2)
    self.value = value
    if self.change_callback then self.change_callback(value) end
end

function Knob:angleFromValue(value)
    local min_angle = 0.5
    local max_angle = 0

    return min_angle + (max_angle - min_angle) * value
end

function Knob:distance(x1, y1, x2, y2)
    local dx = x2 - x1
    local dy = y2 - y1
    return math.sqrt(dx * dx + dy * dy)
end

function Knob:round(num, num_dec_pl)
    local mult = 10 ^ (num_dec_pl or 0)
    return flr(num * mult + 0.5) / mult
end

return Knob
