-- Viewport module
local Viewport = {}
Viewport.__index = Viewport

function Viewport.new(x, y)
    local self = setmetatable({}, Viewport)

    self.x = x
    self.y = y
    self.dx = 0
    self.dy = 0
    self.currentTool = "draw"
    self.drawGrid = false
    self.permanentGrid = false
    self.dragging = false
    self.scrollX = 0
    self.scrollY = 0
    self.mouseX = 0
    self.mouseY = 0
    self.viewportWidth = 160
    self.viewportHeight = 120 - self.x -- - 64
    self.spriteIndex = 0

    self.history = {}
    self.historyIndex = 0

    return self
end

function Viewport:draw()
    -- Draw the viewport area
    local startMapX = math.floor(self.scrollX / 8)
    local endMapX = math.ceil((self.scrollX + self.viewportWidth) / 8)
    local startMapY = math.floor(self.scrollY / 8)
    local endMapY = math.ceil((self.scrollY + self.viewportHeight) / 8)

    cls()

    self:drawBounds()

    for i = 0, 3 do palt(0, false) end
    for mapY = startMapY, endMapY do
        for mapX = startMapX, endMapX do
            local spriteIndex = mget(mapX, mapY)
            if spriteIndex >= 0 then
                local spriteX = self.x + mapX * 8 - self.scrollX
                local spriteY = self.y + mapY * 8 - self.scrollY

                spr(spriteIndex, spriteX, spriteY)
                if self.drawGrid or self.permanentGrid then
                    rect(spriteX - 1, spriteY - 1, 9, 9, 1)
                end
            end
        end
    end
    palt()

    -- draw selection
    if self.mouseX >= self.x and self.mouseX < self.x + self.viewportWidth and
        self.mouseY >= self.y and self.mouseY < self.y + self.viewportHeight then
        local mapX = math.floor((self.mouseX - self.x + self.scrollX) / 8)
        local mapY = math.floor((self.mouseY - self.y + self.scrollY) / 8)
        rect(self.x + mapX * 8 - self.scrollX - 1, self.y + mapY * 8 - self.scrollY - 1,
             9, 9, 2)

        -- Draw tooltip
        self:drawTooltip(mapX, mapY)
    end
end

-- This function draw diagonal lines when out of bounds
function Viewport:drawBounds()
    local mapWidth = 128 * 8 -- Width of the map in pixels
    local mapHeight = 64 * 8 -- Height of the map in pixels

    -- Fill screen with diagonal lines
    for y = 0, 120, 4 do
        for x = 0, 160, 4 do
            -- Convert screen coordinates to map coordinates by adding scroll position
            local mapX = x + self.scrollX
            local mapY = y + self.scrollY

            -- Check if we're outside the map boundaries
            if mapX < 0 or mapX >= mapWidth or mapY < 0 or mapY >= mapHeight then
                -- Draw diagonal lines for out-of-bounds areas
                line(x, y + 8, x + 8, y, 1)
                line(x, y + 7, x + 8, y - 1, 1)
            end
        end
    end
end

function Viewport:drawTooltip(mapX, mapY)
    local tooltipText = str(mapX) .. ":" .. str(mapY)
    local tileX = self.x + mapX * 8 - self.scrollX
    local tileY = self.y + mapY * 8 - self.scrollY
    -- change placement of tooltip next to viewport borders
    if tileX + #tooltipText * 4 + 2 > self.x + self.viewportWidth then
        tileX = tileX - #tooltipText * 4 - 2 - 8
    end

    if tileY - 8 < self.y then tileY = tileY + 20 end

    rectfill(tileX + 8, tileY - 8, #tooltipText * 4 + 2, 7, 2)
    print(tooltipText, tileX + 9, tileY - 7, 0)
end

function Viewport:key(key_code, ctrl_pressed, shift_pressed)
    self.currentTool = "draw"
    if ctrl_pressed and key_code == KEYCODE.KEY_Z then
        if shift_pressed then
            -- Redo
            if self.historyIndex < #self.history then
                self.historyIndex = self.historyIndex + 1
                local action = self.history[self.historyIndex]
                mset(action.x, action.y, action.newSpriteIndex)
            end
        else
            -- Undo
            if self.historyIndex > 0 then
                local action = self.history[self.historyIndex]
                mset(action.x, action.y, action.prevSpriteIndex)
                self.historyIndex = self.historyIndex - 1
            end
        end
    elseif key_code == KEYCODE.KEY_SPACE then
        self.drawGrid = true
        self.currentTool = "drag"
    end
end

function Viewport:keyup(key_code, ctrl_pressed, shift_pressed)
    -- Handle key release events
    if key_code == KEYCODE.KEY_SPACE then
        self.drawGrid = false
        self.currentTool = "draw"
    end
end

function Viewport:mousep(x, y, button)
    if button ~= 1 then return end

    if self.currentTool == "draw" then
        local mapX = math.floor((x - self.x + self.scrollX) / 8)
        local mapY = math.floor((y - self.y + self.scrollY) / 8)
        local prevSpriteIndex = mget(mapX, mapY)
        mset(mapX, mapY, self.spriteIndex) -- Set sprite number, for example, 2

        trace("mapX: " .. mapX .. ", mapY: " .. mapY .. ", sprite: " ..
                  prevSpriteIndex)

        -- Record the action in the history stack
        self.historyIndex = self.historyIndex + 1
        self.history[self.historyIndex] = {
            x = mapX,
            y = mapY,
            prevSpriteIndex = prevSpriteIndex,
            newSpriteIndex = self.spriteIndex
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

function Viewport:mouser(x, y, button)
    -- Handle mouse release events
    if button == 1 then
        self.dragging = false -- Ensure dragging is stopped when mouse button is released
    end
end

function Viewport:mousem(x, y)
    self.mouseX = x
    self.mouseY = y

    -- Handle mouse movement events
    if self.dragging and self.currentTool == "drag" then
        local deltaX = x - self.prevMouseX
        local deltaY = y - self.prevMouseY
        self.scrollX = self.scrollX - deltaX
        self.scrollY = self.scrollY - deltaY
    end
    self.prevMouseX = x
    self.prevMouseY = y
end

return Viewport
