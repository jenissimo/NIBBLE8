local ModControl = {}

function ModControl:init()
    self.keys = {}
    -- oct 2
    self.keys[KEYCODE.KEY_Z] = 12
    self.keys[KEYCODE.KEY_S] = 13
    self.keys[KEYCODE.KEY_X] = 14
    self.keys[KEYCODE.KEY_D] = 15
    self.keys[KEYCODE.KEY_C] = 16
    self.keys[KEYCODE.KEY_V] = 17
    self.keys[KEYCODE.KEY_G] = 18
    self.keys[KEYCODE.KEY_B] = 19
    self.keys[KEYCODE.KEY_H] = 20
    self.keys[KEYCODE.KEY_N] = 21
    self.keys[KEYCODE.KEY_J] = 22
    self.keys[KEYCODE.KEY_M] = 23
    self.keys[KEYCODE.KEY_COMMA] = 24
    -- oct 3
    self.keys[KEYCODE.KEY_Q] = 24
    self.keys[KEYCODE.KEY_2] = 25
    self.keys[KEYCODE.KEY_W] = 26
    self.keys[KEYCODE.KEY_3] = 27
    self.keys[KEYCODE.KEY_E] = 28
    self.keys[KEYCODE.KEY_R] = 29
    self.keys[KEYCODE.KEY_5] = 30
    self.keys[KEYCODE.KEY_T] = 31
    self.keys[KEYCODE.KEY_6] = 32
    self.keys[KEYCODE.KEY_Y] = 33
    self.keys[KEYCODE.KEY_7] = 34
    self.keys[KEYCODE.KEY_U] = 35

    self:getSamples()
end

function ModControl:getSamples()
    local samples = {}
    local startAddress = 0x7C18
    local offset = 40
    -- trace((str(0x7C40-startAddress)))

    for sample = 0, 15 do
        local name = string.format("%x ", (sample + 1))
        for i = 0, 21 do
            name = name .. chr(peek(startAddress + sample * offset + i))
        end
        trace(name)
        samples[sample + 1] = name
    end

    return samples
end

function ModControl:getSampleName(sample)
    local name = ""
    local startAddress = 0x7C18

    for i = 0, 21 do
        name = name .. chr(peek(startAddress + sample * offset + i))
    end
    trace(name)
end

function ModControl:handleKeyInput(tracker, key_code)
    -- TODO: check tracker.editMode
    if self.keys[key_code] then
        self:playNote(self.keys[key_code], tracker.currentInstrument, 63)
    end
end

function ModControl:playNote(note, sample, volume)
    trace("Play note: " .. str(note) .. " sample: " .. sample)
    -- note
    poke(0x7bfc, note)
    -- sample
    poke(0x7bfd, sample)
    -- volume
    poke(0x7bfe, volume)
    -- active flag with reset
    poke(0x7bff, 3)
end

return ModControl
