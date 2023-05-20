local UIManager = require("ui/UIManager")
local Label = require("ui/Label")
local DrawArea = require("os/apps/sprite_editor/DrawArea")
local ColorPicker = require("os/apps/sprite_editor/ColorPicker")
local SpriteSelector = require("os/apps/sprite_editor/SpriteSelector")
local TabBar = require("os/apps/sprite_editor/TabBar")

local sprite_editor = { x = 0, y = 7, w = 159, h = 113 }
local uiManager
local spriteNumberLabel
local drawArea
local colorPicker
local spriteSelector
local spriteSelectorTabBar

function sprite_editor:init()
    cls()
    uiManager = UIManager.new()
    spriteNumberLabel = Label.new(63, 1, "#001", 1)
    drawArea = DrawArea.new(sprite_editor.x + 4, sprite_editor.y + 4, 64, 64)
    colorPicker = ColorPicker.new(sprite_editor.x + 25, sprite_editor.y + 89)
    spriteSelector = SpriteSelector.new(80, 0, 80, 120)
    spriteSelectorTabBar = TabBar.new(73, sprite_editor.y + 2, 2)
    uiManager:addElement(spriteNumberLabel)
    uiManager:addElement(drawArea)
    uiManager:addElement(colorPicker)
    uiManager:addElement(spriteSelector)
    uiManager:addElement(spriteSelectorTabBar)
end

function sprite_editor:update()
    drawArea.currentSprite = spriteSelector.selectedSprite
    drawArea.currentColor = colorPicker.selectedColor
    drawArea:setZoom(1)
    spriteSelector.page = spriteSelectorTabBar.selectedTab
    spriteSelector:setZoom(1)

    uiManager:update()
end

function sprite_editor:draw()
    --rectfill(self.x,self.y,self.width,self.height,0)
    rectfill(sprite_editor.x,sprite_editor.y,79,113,1)
    --local cols = {2,3}
    --for x = 0,9 do
    --    for y = 0,13 do
    --        local colIndex = (x%2+y%2+1)%2
    --        if x % 2 == 0 then
    --            rectfill(79+x*8,sprite_editor.y+y*8,8,8,cols[colIndex])
    --        else
    --            rectfill(79+x*8,sprite_editor.y+y*8,8,8,cols[colIndex])
    --        end
    --    end
    --end

    if spriteSelector.selectedSprite > 99 then    
        spriteNumberLabel.text = "#" .. spriteSelector.selectedSprite
    elseif spriteSelector.selectedSprite > 9 then
        spriteNumberLabel.text = "#0" .. spriteSelector.selectedSprite
    else
        spriteNumberLabel.text = "#00" .. spriteSelector.selectedSprite
    end
    --spriteNumberLabel.x = (sprite_editor.x + 39) - #spriteNumberLabel.text*2
    uiManager:draw()
end

function sprite_editor:mousep(x, y, button)
    uiManager:mousepressed(x, y, button)
end

function sprite_editor:mouser(x, y, button)
    uiManager:mousereleased(x, y, button)
end

function sprite_editor:mousem(x, y)
    uiManager:mousemoved(x, y)
end

return sprite_editor