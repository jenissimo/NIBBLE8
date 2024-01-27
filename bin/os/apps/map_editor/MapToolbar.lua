-- Toolbar module
local MapToolbar = {}
MapToolbar.__index = MapToolbar

function MapToolbar.new()
    local self = setmetatable({}, MapToolbar)
    -- Initialize Toolbar properties
    self.xOffset = 160
    self.spiteSelectorToggle = false
    self.showGridToggle = false
    self.elements={}
    self.elements[1] = {
        spr = {
            {0, 1, 0, 1, 0},
            {1, 1, 1, 1, 1},
            {0, 1, 0, 1, 0},
            {1, 1, 1, 1, 1},
            {0, 1, 0, 1, 0},
        },
        type = "toggle",
        switch = "showGridToggle"
    }
    self.elements[2] = {
        spr = {
            {0, 1, 0, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 1, 1, 0, 0},
            {0, 1, 0, 0, 0},
        },
        toggledSpr = {
            {0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0}, 
        },
        type = "toggle",
        switch = "spiteSelectorToggle"
    }
    return self
end

function MapToolbar:draw()
    -- draw icons for toolbar horizontally
    local x = self.xOffset - 8 * #self.elements
    local y = 0
    
    for i, element in ipairs(self.elements) do
        local glyph
        if element.type == "toggle" then
            if self[element.switch] and element.toggledSpr then
                glyph = element.toggledSpr
            else 
                glyph = element.spr
            end
        end

        for j, row in ipairs(glyph) do
            for k, col in ipairs(row) do
                if col == 1 then
                    if element.type == "toggle" then
                        if self[element.switch] then
                            pset(x + k, y + j, 3)
                        else
                            pset(x + k, y + j, 1)
                        end
                    end
                end
            end
        end
        x = x + 8
    end
end

function MapToolbar:mousep(mouseX, mouseY, button)
    -- Assuming the button we care about is the left mouse button
    if button ~= 1 then return end

    -- Calculate the initial x position based on the toolbar's placement and element size
    local x = self.xOffset - 8 * #self.elements
    local y = 0
    local elementWidth = 7 -- Width of an element sprite
    local elementHeight = 7 -- Height of an element sprite

    -- Iterate over each element to see if the click was within its bounds
    for i, element in ipairs(self.elements) do
        local elementX = x + (i - 1) * 8 -- Position of the element based on its index
        local elementY = y -- y position is constant as toolbar elements are aligned horizontally

        -- Check if the mouse click is within the bounds of the element
        if mouseX >= elementX and mouseX < elementX + elementWidth and
           mouseY >= elementY and mouseY < elementY + elementHeight then
            -- Toggle the corresponding switch
            if element.type == "toggle" then
                self[element.switch] = not self[element.switch]
                if element.switch == "spiteSelectorToggle" then
                    if self[element.switch] then
                        self.xOffset = 80
                    else
                        self.xOffset = 160
                    end
                end
            end
            break -- A click can only interact with one element
        end
    end
end

-- Add other necessary functions for the toolbar

return MapToolbar
