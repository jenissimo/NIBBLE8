local Channel = require("os/apps/sfx/Channel")

local MusicTracker = {}
MusicTracker.__index = MusicTracker

function MusicTracker.new(x,y)
    local self = setmetatable({}, MusicTracker)
    self.x = x
    self.y = y

    self.channels = {}
    for i = 1, 5 do
        self.channels[i] = Channel.new(i, (i - 1)*32 + (i-1)*2 - 1, self.y)
    end

    self.currentTrack = 1
    self.currentNote = 1
    self.cursorX = 0
    self.cursorY = 0
    self.selectedField = "noteIndex"

    return self
end

function MusicTracker:draw()
    rectfill(0, 7, 159, 113, 0)
    -- Draw the tracker interface here
    for i = 1, 5 do
        (self.channels[i]):draw()
    end
end

function MusicTracker:update(dt)
    -- Update the tracker state, if needed
end

function MusicTracker:key(key_code, ctrl_pressed, shift_pressed)
    -- Handle key press events
    for i = 1, 5 do
        self.channels[i]:key(key_code, ctrl_pressed, shift_pressed)
    end
end

function MusicTracker:keyup(key_code, ctrl_pressed, shift_pressed)
    -- Handle key release events
end

return MusicTracker
