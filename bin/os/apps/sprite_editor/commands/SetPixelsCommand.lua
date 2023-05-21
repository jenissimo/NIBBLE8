local Command = require("os/apps/sprite_editor/commands/Command")

local SetPixelsCommand = setmetatable({}, Command)
SetPixelsCommand.__index = SetPixelsCommand

function SetPixelsCommand.new(drawArea, oldPixels)
    local self = setmetatable(Command.new(), SetPixelsCommand)
    self.drawArea = drawArea
    self.oldPixels = oldPixels
    self.newPixels = {}
    return self
end

function SetPixelsCommand:setNewPixels(newPixels)
    self.newPixels = newPixels
    trace(#self.newPixels)
end

function SetPixelsCommand:execute()
    local sprSize = self.drawArea:getSpriteSize()
    for y = 1, sprSize do
        for x = 1, sprSize do
            self.drawArea:setPixelLocal(x - 1, y - 1, self.newPixels[y][x])
        end
    end
end

function SetPixelsCommand:undo()
    trace("SetPixelsCommand:undo()")
    local sprSize = self.drawArea:getSpriteSize()
    for y = 1, sprSize do
        for x = 1, sprSize do
            self.drawArea:setPixelLocal(x - 1, y - 1, self.oldPixels[y][x])
        end
    end
end

return SetPixelsCommand
