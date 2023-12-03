local UIElement = require("ui/UIElement")
local ToggleSwitch = setmetatable({}, UIElement)
ToggleSwitch.__index = ToggleSwitch

function ToggleSwitch.new(x, y, width, height, state, label, change_callback)
    local self = setmetatable(UIElement.new(x, y, width, height), ToggleSwitch)
    self.state = state
    self.label = label
    self.change_callback = change_callback
    return self
end

function ToggleSwitch:draw()
    local toggle_width = self.height -- width of the toggle itself
    local toggle_padding = 2 -- padding inside the switch background
    local toggle_color = self.state and 3 or 1 -- color of the toggle, change as needed

    -- Draw toggle background
    rectfill(self.x, self.y, self.width, self.height, 1)

    if self.state then
        rectfill(self.x + toggle_padding, self.y + toggle_padding,
                 toggle_width - toggle_padding * 2,
                 self.height - toggle_padding * 2, toggle_color)
    end
    -- Draw the toggle based on the current state
    local toggle_x =
        self.state and (self.x - toggle_width - toggle_padding * 2) or
            (self.x + toggle_padding)

    -- If the switch has a label, display it
    if self.label then
        local label_x = self.x + (self.width / 2) - (#self.label * 4 / 2)
        local label_y = self.y + self.height
        print(self.label, label_x, label_y, 3)
    end
end

function ToggleSwitch:mousepressed(x, y, button)
    -- Check if the click is within the bounds of the toggle switch
    if x >= self.x and x <= self.x + self.width and y >= self.y and y <= self.y +
        self.height then
        self:toggle()
        if self.change_callback then self.change_callback(self.state) end
    end
end

function ToggleSwitch:toggle() self.state = not self.state end

return ToggleSwitch
