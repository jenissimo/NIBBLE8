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

    local spritesPerRow = 30 -- Total sprites per row in the entire spritesheet
    local pageSpritesPerRow = 10 -- Sprites per row on each page
    local pageSpritesPerColumn = 15 -- Sprites per column on each page

    local pagesPerRow = 3 -- Total pages per row in the spritesheet
    local startCol = (self.page % pagesPerRow) * pageSpritesPerRow
    local startRow = math.floor(self.page / pagesPerRow) * pageSpritesPerColumn

    for row = 0, pageSpritesPerColumn - 1 do
        for col = 0, pageSpritesPerRow - 1 do
            local spriteIndex = (startRow + row) * spritesPerRow +
                                    (startCol + col)
            local x = col * spriteSize + self.x
            local y = row * spriteSize + self.y

            spr(spriteIndex, x, y)
        end
    end

    palt()

    -- Draw selected sprite
    local startCol = (self.page % pagesPerRow) * pageSpritesPerRow
    local startRow = math.floor(self.page / pagesPerRow) * pageSpritesPerColumn
    local endRow = startRow + pageSpritesPerColumn - 1
    local endCol = startCol + pageSpritesPerRow - 1

    local selectedRow = math.floor(self.selectedSprite / spritesPerRow)
    local selectedCol = self.selectedSprite % spritesPerRow

    -- Check if the selected sprite is on the current page
    if selectedRow >= startRow and selectedRow <= endRow and selectedCol >=
        startCol and selectedCol <= endCol then
        local selectedRowRelative = selectedRow - startRow
        local selectedColRelative = selectedCol - startCol

        rect(self.x + selectedColRelative * 8, self.y + selectedRowRelative * 8,
             self.spriteSize, self.spriteSize, 0)
        rect(self.x + selectedColRelative * 8 - 1,
             self.y + selectedRowRelative * 8 - 1, self.spriteSize + 2,
             self.spriteSize + 2, 3)
    end
end

function SpriteSelector:mousepressed(x, y, button)
    if x < self.x or x > self.x + self.width or y < self.y or y > self.y +
        self.height then return end

    if button == 1 then
        local spriteSize = 8
        local spritesPerRow = 30 -- Total sprites per row in the entire spritesheet
        local pageSpritesPerRow = 10 -- Sprites per row on each page
        local pageSpritesPerColumn = 15 -- Sprites per column on each page

        local pagesPerRow = 3 -- Total pages per row in the spritesheet

        local row = math.floor((y - self.y) / spriteSize)
        local col = math.floor((x - self.x) / spriteSize)

        local startCol = (self.page % pagesPerRow) * pageSpritesPerRow
        local startRow = math.floor(self.page / pagesPerRow) *
                             pageSpritesPerColumn

        local spriteIndex = (startRow + row) * spritesPerRow + (startCol + col)
        self.selectedSprite = flr(spriteIndex)
    end
end

return SpriteSelector
