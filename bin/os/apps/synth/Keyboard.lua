-- Keyboard
local UIElement = require("ui/UIElement")
local KeyboardUtils = require("os/apps/synth/KeyboardUtils")

local Keyboard = setmetatable({}, Keyboard)
Keyboard.__index = Keyboard

function Keyboard.new(x, y, octaves)
    local self = setmetatable(UIElement.new(x, y,
                                            4 * 7 * octaves + 7 * octaves + 3,
                                            21), Keyboard)
    self.octaves = octaves
    self.currentOctave = 1
    self.minOctave = 0
    self.maxOctave = 3
    return self
end

function Keyboard:drawKeys()
    local blackKeyWidth = 3
    local blackKeyHeight = 10
    local blackKeySpacing = 2
    local whiteKeyWidth = 4
    local whiteKeyHeight = 17
    local whiteKeySpacing = 1
    local xOffset = self.x + 2
    local yOffset = self.y + 2
    local blackKeyXOffsets = {}
    local blackKeyIndices = {}

    -- draw white keys
    for i, key in ipairs(KeyboardUtils.keys) do
        local color, note = table.unpack(key)

        if color == 'white' then
            if self.pressed_key == note then
                rectfill(xOffset, yOffset, whiteKeyWidth, whiteKeyHeight, 2)
            else
                rectfill(xOffset, yOffset, whiteKeyWidth, whiteKeyHeight, 3)
            end
            rectfill(xOffset, yOffset + whiteKeyHeight - 2, whiteKeyWidth, 2, 2)

            xOffset = xOffset + whiteKeyWidth + whiteKeySpacing
        else
            table.insert(blackKeyXOffsets, xOffset - blackKeyWidth / 2)
            table.insert(blackKeyIndices, i)
        end
    end

    -- draw black keys
    for i, xOffset in ipairs(blackKeyXOffsets) do
        if self.pressed_key == KeyboardUtils.keys[blackKeyIndices[i]][2] then
            rectfill(xOffset, yOffset, blackKeyWidth, blackKeyHeight, 1)
        else
            rectfill(xOffset, yOffset, blackKeyWidth, blackKeyHeight, 0)
        end
        rectfill(xOffset, yOffset + blackKeyHeight - 1, blackKeyWidth, 1, 1)
    end
end

function Keyboard:key(key_code, ctrl_pressed, shift_pressed)
    local key = KeyboardUtils.keyboardToPiano[key_code]
    if key and key ~= self.pressed_key then
        --trace(key)
        self.pressed_key = key
        local octave = self.currentOctave + flr(key / 12) + 2
        note_on(key % 12, octave)
    end
end

function Keyboard:keyup(key_code, ctrl_pressed, shift_pressed)
    local key = KeyboardUtils.keyboardToPiano[key_code]
    if key and key == self.pressed_key then
        self.pressed_key = nil
        --trace(key)
        note_off()
    end
end

function Keyboard:draw()
    rect(self.x, self.y, self.width, self.height, 1)
    self:drawKeys()
end

return Keyboard
