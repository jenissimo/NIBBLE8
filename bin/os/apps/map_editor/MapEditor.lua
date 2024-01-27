local MapEditor = {}
MapEditor.__index = MapEditor

local Viewport = require("os/apps/map_editor/Viewport")
local MapToolbar = require("os/apps/map_editor/MapToolbar")
local SpriteSelector = require("os/apps/sprite_editor/SpriteSelector")
local TabBar = require("os/apps/sprite_editor/TabBar")

function MapEditor.new(x, y)
    local self = setmetatable({}, MapEditor)
    self.viewport = Viewport.new(x,y)
    self.x = x
    self.y = y
    self.toolbar = MapToolbar.new()
    self.spriteSelector = SpriteSelector.new(80, 0, 80, 120)
    self.spriteSelectorTabBar = TabBar.new(73, y + 2, 6)

    return self
end

function MapEditor:update()
    -- Update the MapEditor state
    -- self.viewport:update()
    if self.toolbar.spiteSelectorToggle then
        self.spriteSelector.page = self.spriteSelectorTabBar.selectedTab
    end

    self.viewport.permanentGrid = self.toolbar.showGridToggle
    self.viewport.spriteIndex = self.spriteSelector.selectedSprite
end

function MapEditor:draw()
    self.viewport:draw()
end

function MapEditor:drawPost()
    self.toolbar:draw()
    if self.toolbar.spiteSelectorToggle then
        rectfill(73, self.y -1, 6, 54, 0)
        self.spriteSelector:draw()
        self.spriteSelectorTabBar:draw()
    end
end

function MapEditor:key(key_code, ctrl_pressed, shift_pressed)
    self.viewport:key(key_code, ctrl_pressed, shift_pressed)    
end

function MapEditor:keyup(key_code, ctrl_pressed, shift_pressed)
    self.viewport:keyup(key_code, ctrl_pressed, shift_pressed)
end

function MapEditor:isMouseOver(obj, mouseX, mouseY)
    return mouseX >= obj.x and mouseX <= obj.x + obj.width and mouseY >= obj.y and mouseY <= obj.y + obj.height
end

function MapEditor:isNeedToPreventViewportEvents(mouseX, mouseY)
    if not self.toolbar.spiteSelectorToggle then
        return false
    end

    if self:isMouseOver(self.spriteSelector, mouseX, mouseY) then
        return true
    end

    if self:isMouseOver(self.spriteSelectorTabBar, mouseX, mouseY) then
        return true
    end

    return false
end

function MapEditor:mousep(x, y, button)
    if self.toolbar.spiteSelectorToggle then
        self.spriteSelector:mousepressed(x, y, button)
        self.spriteSelectorTabBar:mousepressed(x, y, button)
    end
    
    if not self:isNeedToPreventViewportEvents(x, y) then
        self.viewport:mousep(x, y, button)
    end
    self.toolbar:mousep(x, y, button)
end

function MapEditor:mouser(x, y, button)
    if not self:isNeedToPreventViewportEvents(x, y) then
        self.viewport:mouser(x, y, button)
    end
    self.spriteSelectorTabBar:mousereleased(x, y, button)
end

function MapEditor:mousem(x, y)
    if not self:isNeedToPreventViewportEvents(x, y) then
        self.viewport:mousem(x, y)
    end
end

return MapEditor
