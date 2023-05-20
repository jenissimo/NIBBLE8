local MapEditor = {}
MapEditor.__index = MapEditor

local KEYCODE = {KEY_Z = "z"}

function MapEditor.drawPencilIcon(x, y, color)
    rectfill(x, y + 5, 13, 5, color)
    line(x + 12, y + 4, x + 15, y + 1, color)
    line(x + 12, y + 5, x + 15, y + 1, color)
    line(x + 12, y + 4, x + 15, y + 8, color)
    line(x + 12, y + 5, x + 15, y + 8, color)
end

function MapEditor.drawHandIcon(x, y, color)
    rectfill(x + 5, y + 10, 6, 6, color)
    line(x + 5, y + 10, x + 2, y + 7, color)
    line(x + 6, y + 10, x + 3, y + 7, color)
    line(x + 10, y + 10, x + 13, y + 7, color)
    line(x + 11, y + 10, x + 14, y + 7, color)
end

function MapEditor.new(x, y)
    local self = setmetatable({}, MapEditor)
    self.x = x
    self.y = y
    self.dx = 0
    self.dy = 0
    self.viewportWidth = 160
    self.viewportHeight = 120 - self.x-- - 64
    self.toolbarHeight = 64
    self.currentTool = "draw"
    self.dragging = false
    self.scrollX = 0
    self.scrollY = 0
    self.mouseX = 0
    self.mouseY = 0

    self.history = {}
    self.historyIndex = 0

    self.toolbarButtons = {
        {
            draw = self.drawPencilIcon,
            x = x + 8,
            y = y + self.viewportHeight + 4,
            width = 16,
            height = 16,
            tool = "draw"
        }, {
            draw = self.drawHandIcon,
            x = x + 32,
            y = y + self.viewportHeight + 4,
            width = 16,
            height = 16,
            tool = "drag"
        }
    }

    self.history = {}
    self.historyIndex = 0

    return self
end

function MapEditor:update()
    -- Update the MapEditor state
end

function MapEditor:draw()
    -- Draw the viewport area
    local startMapX = math.floor(self.scrollX / 8)
    local endMapX = math.ceil((self.scrollX + self.viewportWidth) / 8)
    local startMapY = math.floor(self.scrollY / 8)
    local endMapY = math.ceil((self.scrollY + self.viewportHeight) / 8)

    rectfill(0, 7, 159, 113, 0)

    for mapY = startMapY, endMapY do
        for mapX = startMapX, endMapX do
            local spriteIndex = mget(mapX, mapY)
            local spriteX = (spriteIndex % 32) * 8
            local spriteY = math.floor(spriteIndex / 32) * 8
            sspr(spriteX, spriteY, 8, 8, self.x + mapX * 8 - self.scrollX,
                 self.y + mapY * 8 - self.scrollY, 8, 8, false, false)
        end
    end

    -- Draw grid
    for i = 1, self.viewportWidth, 8 do
        line(self.x + i, self.y, self.x + i, self.y + self.viewportHeight, 1)
    end
    for i = 1, self.viewportHeight, 8 do
        line(self.x, self.y + i, self.x + self.viewportWidth, self.y + i, 1)
    end

    if self.mouseX >= self.x and self.mouseX < self.x + self.viewportWidth and
        self.mouseY >= self.y and self.mouseY < self.y + self.viewportHeight then
        local mapX = math.floor((self.mouseX - self.x + self.scrollX) / 8)
        local mapY = math.floor((self.mouseY - self.y + self.scrollY) / 8)
        rect(self.x + mapX * 8 - self.scrollX + 1, self.y + mapY * 8 - self.scrollY + 1,
             9, 9, 2)
        
        -- Draw tooltip
        self:drawTooltip(mapX, mapY)
    end

    -- Draw the toolbar UI
    --rectfill(self.x, self.y + self.viewportHeight, self.viewportWidth,
    --         self.toolbarHeight, 3)
    --for _, button in ipairs(self.toolbarButtons) do
    --    button.draw(button.x, button.y, 1)
    --end

end

function MapEditor:drawTooltip(mapX, mapY)
    local tooltipText = str(mapX)..":"..str(mapY)
    local tileX = self.x + mapX * 8 - self.scrollX
    local tileY = self.y + mapY * 8 - self.scrollY
    -- change placement of tooltip next to viewport borders
    if tileX + #tooltipText * 4 + 2 > self.x + self.viewportWidth then
        tileX = tileX - #tooltipText * 4 - 2 - 8
    end

    if tileY - 8 < self.y then
        tileY = tileY + 20
    end

    rectfill(tileX + 8, tileY - 8, #tooltipText * 4 + 2, 7, 2)
    print(tooltipText, tileX + 9, tileY - 7, 0)
end

function MapEditor:key(key_code, ctrl_pressed, shift_pressed)
    if ctrl_pressed and key_code == KEYCODE.KEY_Z then
        if shift_pressed then
            -- Redo
            if self.historyIndex < #self.history then
                self.historyIndex = self.historyIndex + 1
                local action = self.history[self.historyIndex]
                nibble_api_mset(action.x, action.y, action.newSpriteIndex)
            end
        else
            -- Undo
            if self.historyIndex > 0 then
                local action = self.history[self.historyIndex]
                nibble_api_mset(action.x, action.y, action.prevSpriteIndex)
                self.historyIndex = self.historyIndex - 1
            end
        end
    end
end

function MapEditor:keyup(key_code, ctrl_pressed, shift_pressed)
    -- Handle key release events
end

function MapEditor:mousep(x, y, button)
    if button ~= 1 then return end

    -- Check if a toolbar button was clicked
    for _, button in ipairs(self.toolbarButtons) do
        if x >= button.x and x < button.x + button.width and y >= button.y and y <
            button.y + button.height then self.currentTool = button.tool end
    end

    if self.currentTool == "draw" then
        local mapX = math.floor((x - self.x + self.scrollX) / 8)
        local mapY = math.floor((y - self.y + self.scrollY) / 8)
        local prevSpriteIndex = nibble_api_mget(mapX, mapY)
        nibble_api_mset(mapX, mapY, 1) -- Set sprite number, for example, 1

        -- Record the action in the history stack
        self.historyIndex = self.historyIndex + 1
        self.history[self.historyIndex] = {
            x = mapX,
            y = mapY,
            prevSpriteIndex = prevSpriteIndex,
            newSpriteIndex = 1
        }
        -- Remove any redo actions
        for i = self.historyIndex + 1, #self.history do
            self.history[i] = nil
        end
    elseif self.currentTool == "drag" then
        self.dragging = true
        self.prevMouseX = x
        self.prevMouseY = y
    end
end

function MapEditor:mouser(x, y, button)
    -- Handle mouse release events
    if button == 1 then
        if self.currentTool == "drag" then self.dragging = false end
    end
end

function MapEditor:mousem(x, y)
    self.mouseX = x
    self.mouseY = y

    -- Handle mouse movement events
    if self.isDragging then
        local deltaX = x - self.prevMouseX
        local deltaY = y - self.prevMouseY
        self.scrollX = self.scrollX - deltaX
        self.scrollY = self.scrollY - deltaY
    end
    self.prevMouseX = x
    self.prevMouseY = y
end

return MapEditor

