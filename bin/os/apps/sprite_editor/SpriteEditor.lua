local UIManager = require("ui/UIManager")
local Label = require("ui/Label")
local DrawArea = require("os/apps/sprite_editor/DrawArea")
local DrawTools = require("os/apps/sprite_editor/DrawTools")
local ColorPicker = require("os/apps/sprite_editor/ColorPicker")
local SpriteSelector = require("os/apps/sprite_editor/SpriteSelector")
local TabBar = require("os/apps/sprite_editor/TabBar")
local SpriteFlagsSelector = require("os/apps/sprite_editor/SpriteFlagsSelector")
local NumbericStepper = require("os/apps/sprite_editor/NumbericStepper")
local CommandStack = require("os/apps/sprite_editor/commands/CommandStack")

local sprite_editor = {x = 0, y = 7, w = 159, h = 113}
local uiManager
local commandStack

local spriteNumberLabel
local drawArea
local drawTools
local colorPicker
local spriteFlagsSelector
local brushSizeStepper
local spriteSizeStepper

local spriteSelector
local spriteSelectorTabBar

function sprite_editor:init()
    cls()
    uiManager = UIManager.new()
    commandStack = CommandStack.new()

    spriteNumberLabel = Label.new(30, 9, "#001", 3)
    drawArea = DrawArea.new(sprite_editor.x + 6, sprite_editor.y + 11, 64, 64, commandStack)
    drawTools = DrawTools.new(drawArea.x + 5, drawArea.y + drawArea.height + 4, sprite_editor.onToolChanged)
    colorPicker = ColorPicker.new(sprite_editor.x + 3, sprite_editor.y + 89)
    spriteSelector = SpriteSelector.new(80, 0, 80, 120)
    spriteSelectorTabBar = TabBar.new(73, sprite_editor.y + 2, 2)
    spriteFlagsSelector = SpriteFlagsSelector.new(16, 105)

    brushSizeStepper = NumbericStepper.new(33, 96, 1, 4,
                                           sprite_editor.onBrushSizeChanged,
                                           function(x, y, value)
        rectfill(x, y, 7, 7, 0)
        if value == 1 then
            pset(x + 3, y + 3, 3)
        elseif value == 2 then
            rectfill(x + 3, y + 3, 2, 2, 3)
        elseif value == 3 then
            rectfill(x + 2, y + 2, 3, 3, 3)
        elseif value == 4 then
            rectfill(x + 1, y + 1, 5, 5, 3)
        end
    end)

    spriteSizeStepper = NumbericStepper.new(55, 96, 1, 4, sprite_editor.onSpriteSizeChanged,
                                            function(x, y, value)
        rectfill(x, y, 7, 7, 0)
        if value == 1 then
            pset(x + 2, y + 2, 3)
            pset(x + 4, y + 2, 3)
            pset(x + 2, y + 4, 3)
            pset(x + 4, y + 4, 3)
        elseif value == 2 then
            line(x + 1, y + 2, x + 2, y + 1, 3)
            line(x + 4, y + 1, x + 5, y + 2, 3)
            line(x + 1, y + 4, x + 2, y + 5, 3)
            line(x + 4, y + 5, x + 5, y + 4, 3)
        elseif value == 3 then
            -- left top corner
            line(x + 1, y + 1, x + 2, y + 1, 3)
            line(x + 1, y + 1, x + 1, y + 2, 3)

            -- right top corner
            line(x + 5, y + 1, x + 4, y + 1, 3)
            line(x + 5, y + 1, x + 5, y + 2, 3)

            -- left bottom corner
            line(x + 1, y + 5, x + 1, y + 4, 3)
            line(x + 1, y + 5, x + 2, y + 5, 3)

            -- right bottom corner
            line(x + 5, y + 5, x + 4, y + 5, 3)
            line(x + 5, y + 5, x + 5, y + 4, 3)
        elseif value == 4 then
            rect(x + 1, y + 1, 5, 5, 3)
        end
    end)

    uiManager:addElement(spriteNumberLabel)
    uiManager:addElement(spriteSizeStepper)
    uiManager:addElement(drawArea)
    uiManager:addElement(drawTools)
    uiManager:addElement(colorPicker)
    uiManager:addElement(brushSizeStepper)
    uiManager:addElement(spriteFlagsSelector)

    uiManager:addElement(spriteSelector)
    uiManager:addElement(spriteSelectorTabBar)
end

function sprite_editor:update()
    drawArea.currentSprite = spriteSelector.selectedSprite
    drawArea.currentColor = colorPicker.selectedColor
    spriteSelector.page = spriteSelectorTabBar.selectedTab

    uiManager:update()
end

function sprite_editor:drawPost()
    rectfill(sprite_editor.x, sprite_editor.y, 79, 113, 1)

    if spriteSelector.selectedSprite > 99 then
        spriteNumberLabel.text = "#" .. spriteSelector.selectedSprite
    elseif spriteSelector.selectedSprite > 9 then
        spriteNumberLabel.text = "#0" .. spriteSelector.selectedSprite
    else
        spriteNumberLabel.text = "#00" .. spriteSelector.selectedSprite
    end
    uiManager:draw()
    self:drawStatusBar()
end

function sprite_editor.onToolChanged(tool)
    drawArea.currentTool = tool
end

function sprite_editor.onBrushSizeChanged(value)
    -- drawArea:setBrushSize(value)
end

function sprite_editor.onSpriteSizeChanged(value)
    trace("sprite size changed: " .. value)
    drawArea:setZoom(value)
    if value == 1 then
        drawArea:setZoom(1)
        spriteSelector:setZoom(1)
    elseif value == 2 then
        drawArea:setZoom(2)
        spriteSelector:setZoom(2)
    elseif value == 3 then
        drawArea:setZoom(4)
        spriteSelector:setZoom(4)
    elseif value == 4 then
        drawArea:setZoom(8)
        spriteSelector:setZoom(8)
    end
end

function sprite_editor:drawStatusBar()
    local status

    rectfill(0, 113, 79, 7, 2)

    if drawArea.cursorPos ~= nil then
        status = "x: " .. drawArea.cursorPos.x .. " y: " .. drawArea.cursorPos.y
        print(status, 1, 114, 3)
    end
end

function sprite_editor:key(key_code, ctrl_pressed, shift_pressed)
    trace("key pressed: " .. key_code)
    if ctrl_pressed then
        if key_code == KEYCODE.KEY_Z then
            trace("CTRL+Z")
            commandStack:undo()
            return
        end
    end

    if key_code == KEYCODE.KEY_LEFTBRACKET then
        brushSizeStepper:decrease()
        return
    elseif key_code == KEYCODE.KEY_RIGHTBRACKET then
        brushSizeStepper:increase()
        return
    elseif key_code == KEYCODE.KEY_B then
        drawTools:setSelectedTool(1)
        return
    elseif key_code == KEYCODE.KEY_S then
        drawTools:setSelectedTool(2)
        return
    elseif key_code == KEYCODE.KEY_M then
        drawTools:setSelectedTool(3)
        return
    elseif key_code == KEYCODE.KEY_F then
        drawTools:setSelectedTool(4)
        return
    elseif key_code == KEYCODE.KEY_C then
        drawTools:setSelectedTool(5)
        return
    elseif key_code == KEYCODE.KEY_R then
        drawTools:setSelectedTool(6)
        return
    elseif key_code == KEYCODE.KEY_L then
        drawTools:setSelectedTool(7)
        return
    elseif key_code == KEYCODE.KEY_1 then
        colorPicker.selectedColor = 0
        return
    elseif key_code == KEYCODE.KEY_2 then
        colorPicker.selectedColor = 1
        return
    elseif key_code == KEYCODE.KEY_3 then
        colorPicker.selectedColor = 2
        return
    elseif key_code == KEYCODE.KEY_4 then
        colorPicker.selectedColor = 3
        return
    end
end

function sprite_editor:mousep(x, y, button) uiManager:mousepressed(x, y, button) end

function sprite_editor:mouser(x, y, button) uiManager:mousereleased(x, y, button) end

function sprite_editor:mousem(x, y) uiManager:mousemoved(x, y) end

return sprite_editor
