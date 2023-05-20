-- Sprite Selector
local UIElement = require("ui/UIElement")
local SpriteSelector = setmetatable({}, UIElement)
SpriteSelector.__index = SpriteSelector

function SpriteSelector.new(x, y, width, height)
    local self =
        setmetatable(UIElement.new(x, y, width, height), SpriteSelector)
    self.page = 0
    self.selectedSprite = 0
    self:setZoom(1)
    return self
end

function SpriteSelector:setZoom(zoom)
    self.zoom = zoom
    self.zoomCoef = self.width / (self.zoom * 8)
    self.spriteSize = 8 * self.zoom
    self.spritesHCount = self.width / self.spriteSize
    self.spritesVCount = self.height / self.spriteSize
    -- trace("Zoom: "..self.zoom.." - "..self.zoomCoef.." - "..self.spriteSize.." - "..self.spritesHCount.." - "..self.spritesVCount)
end

function SpriteSelector:draw()
    -- Borders
    line(self.x - 1, self.y, self.x - 1, self.y + self.height, 3)

    for i = 0, 3 do palt(0, false) end

    local spriteSize = 8
    local spritesPerRow = 160 / spriteSize
    local columns = flr(self.width / spriteSize) * 2
    local rows = flr(self.height / spriteSize)
    local totalPages = flr(160 / self.width)

    for j = 0, rows - 1 do
        for i = 0, columns - 1 do
            local spriteIndex = self.page * columns * rows + j * spritesPerRow + i
            local x = self.x + i * spriteSize
            local y = self.y + j * spriteSize
            if x <= 152 then
                spr(spriteIndex, x, y)
            end
        end
    end

    palt()

    -- Selected sprite
    local row = flr(self.selectedSprite / spritesPerRow)
    local col = self.selectedSprite % spritesPerRow

    rect(self.x + col * 8, self.y + row * 8, self.spriteSize, self.spriteSize, 0)
    rect(self.x + col * 8 - 1, self.y + row * 8 - 1, self.spriteSize + 2,
         self.spriteSize + 2, 3)
end

function SpriteSelector:mousepressed(x, y, button)
    if x < self.x or x > self.x + self.width or y < self.y or y > self.y +
        self.height then return end

    if button == 1 then
        local spriteSize = 8
        local spritesPerRow = 160 / spriteSize
        local columns = flr(self.width / spriteSize) * 2
        local rows = flr(self.height / spriteSize)
        local totalPages = flr(160 / self.width)

        local row = flr((y - self.y) / spriteSize)
        local col = flr((x - self.x) / spriteSize)

        self.selectedSprite = self.page * columns * rows + row * spritesPerRow + col
        --self.selectedSprite = row * self.spritesHCount + col + self.page *
        --                          self.spritesHCount * self.spritesVCount
    end
end

return SpriteSelector
