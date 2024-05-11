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

    self.NOTES = {
        "c-", "c#", "d-", "d#", "e-", "f-", "f#", "g-", "g#", "a-", "a#", "b-"
    }

    self.keyPressed = -1
    self.patternsAddress = 0x7eda
end

function ModControl:getSamples()
    local samples = {}
    local startAddress = 0x7C12
    local offset = 23
    -- trace((str(0x7C40-startAddress)))

    for sample = 0, 15 do
        local name = string.format("%x ", (sample + 1))
        for i = 0, 21 do
            name = name .. chr(peek(startAddress + sample * offset + i))
        end
        -- trace(name)
        samples[sample + 1] = name
    end

    self:getPattern(0)

    return samples
end

function ModControl:getPattern(pattern)
    local result = {}
    local numChannels = 4
    local bytesInLine = 4
    local patternLength = 64
    local startAddress = self.patternsAddress +
                             (pattern * patternLength * numChannels *
                                 bytesInLine)
    local address = startAddress
    local sample, period, effect, command, params, note, noteIndex, octave
    local bytes = {}
    local str=""

    for line = 1, 64 do
        for channel = 1, 4 do
            for note_byte = 1, 4 do
                bytes[note_byte] = peek(address + note_byte)
            end

            sample = bit32.bor(bit32.band(bytes[1], 0xf0),
                               bit32.rshift(bytes[3], 4))
            period = bit32.bor(bit32.lshift(bit32.band(bytes[1], 0x0f), 8),
                               bytes[2])
            effect = bit32.bor(bit32.lshift(bit32.band(bytes[3], 0x0f), 8),
                               bytes[4])
            command = bit32.rshift(effect, 8) -- Command is the most significant byte
            params = bit32.band(effect, 0xff) -- Parameters are the least significant byte

            if period == 0 then
                noteIndex = -1
                octave = 0
            else
                note = self:periodToNote(period);
                noteIndex = note % 12 + 1
                octave = flr(note / 12) + 3
            end
            table.insert(result, {noteIndex, octave, sample, command, params})

            address = address + 4
        end
    end
    return result
end

function ModControl:setSample(pattern, line, channel, sample)
    local patternLength = 64
    local bytesInLine = 4
    local numChannels = 4
    local startAddress = self.patternsAddress +
                             (pattern * patternLength * numChannels *
                                 bytesInLine)
    local address = startAddress + (line * numChannels * bytesInLine) +
                        (channel * bytesInLine)

    -- Read the existing bytes from memory
    local existingByte1 = peek(address + 1)
    local existingByte3 = peek(address + 3)

    -- Update the sample bytes while preserving other bytes
    local newByte1 = bit32.bor(bit32.band(sample, 0xf0),
                               bit32.band(existingByte1, 0x0f))
    local newByte3 = bit32.bor(bit32.lshift(bit32.band(sample, 0x0f), 4),
                               bit32.band(existingByte3, 0x0f))

    trace("Start address: "..str(startAddress))
    trace("Address: "..str(address))

    -- Write the updated bytes back to memory
    poke(address + 1, newByte1)
    poke(address + 3, newByte3)
    self:setPatternDirty()
end

function ModControl:setPeriod(pattern, line, channel, period)
    local patternLength = 64
    local bytesInLine = 4
    local numChannels = 4
    local startAddress = self.patternsAddress +
                             (pattern * patternLength * numChannels *
                                 bytesInLine)
    local address = startAddress + (line * numChannels * bytesInLine) +
                        (channel * bytesInLine)

    -- Read the existing bytes from memory
    local existingByte1 = peek(address + 1)

    -- Update the period bytes while preserving other bytes
    local newByte1 = bit32.bor(bit32.band(existingByte1, 0xf0),
                               bit32.band(bit32.rshift(period, 8), 0x0f))
    local newByte2 = bit32.band(period, 0xff)

    -- Write the updated bytes back to memory
    poke(address + 1, newByte1)
    poke(address + 2, newByte2)
    self:setPatternDirty()
end

function ModControl:setCommandAndParams(pattern, line, channel, command, params)
    local patternLength = 64
    local bytesInLine = 4
    local numChannels = 4
    local startAddress = self.patternsAddress +
                             (pattern * patternLength * numChannels *
                                 bytesInLine)
    local address = startAddress + (line * numChannels * bytesInLine) +
                        (channel * bytesInLine)

    -- Read the existing bytes from memory
    local existingByte3 = peek(address + 3)

    -- Update the command and parameters bytes while preserving the sample byte
    local newByte3 = bit32.bor(bit32.band(existingByte3, 0xf0),
                               bit32.band(command, 0x0f))
    local newByte4 = params

    -- Write the updated bytes back to memory
    poke(address + 3, newByte3)
    poke(address + 4, newByte4)
    self:setPatternDirty()
end


function ModControl:handleKeyInput(tracker, key_code, pattern, line, channel,
                                   sample)
    if key_code == self.keyPressed then return end

    if self.keys[key_code] then
        local note = self.keys[key_code]

        self.keyPressed = key_code
        self:playNote(note, tracker.currentInstrument, 64)

        if tracker.editMode then
            local period = self:noteToPeriod(note)
            self:setPeriod(pattern, line, channel, period)
            self:setSample(pattern, line, channel, sample)
            tracker:invalidateTrack()
            tracker:moveCursorV(1)
        end
    elseif key_code == KEYCODE.KEY_0 then
        self:setPeriod(pattern, line, channel, 0)
        self:setSample(pattern, line, channel, 0)
        tracker:invalidateTrack()
        tracker:moveCursorV(1)
    end
end

function ModControl:setPatternDirty()
    local flags = self:getMusicFlags()
    flags = self:setBit(flags, 1, 1)
    self:setMusicFlags(flags)
end

function ModControl:checkKeyUp(tracker, key_code)
    if self.keyPressed == key_code then self:stopNote() end
end

function ModControl:playNote(note, sample, volume)
    -- trace("Play note: " .. str(note) .. " sample: " .. sample)
    -- note
    poke(0x7bf8, note)
    -- sample
    poke(0x7bf9, sample)
    -- volume
    poke(0x7bfa, volume)
    -- active flag with reset
    poke(0x7bfb, 3)
end

function ModControl:stopNote()
    -- active flag 0 with reset 1
    poke(0x7bfb, 0)
    self.keyPressed = -1
end

function ModControl:getCurrentPattern() return peek(0x7c04) end

function ModControl:getCurrentLine() return peek(0x7c05) end

function ModControl:isPlaying() return self:isBitSet(self:getMusicFlags(), 0) end

function ModControl:getMusicFlags() return peek(0x7bf4) end

function ModControl:setMusicFlags(value) return poke(0x7bf4, value) end

function ModControl:periodToNote(period)
    local period_to_note_map = {
        [856] = 0,
        [808] = 1,
        [762] = 2,
        [720] = 3,
        [678] = 4,
        [640] = 5,
        [604] = 6,
        [570] = 7,
        [538] = 8,
        [508] = 9,
        [480] = 10,
        [453] = 11,
        [428] = 12,
        [404] = 13,
        [381] = 14,
        [360] = 15,
        [339] = 16,
        [320] = 17,
        [302] = 18,
        [285] = 19,
        [269] = 20,
        [254] = 21,
        [240] = 22,
        [226] = 23,
        [214] = 24,
        [202] = 25,
        [190] = 26,
        [180] = 27,
        [170] = 28,
        [160] = 29,
        [151] = 30,
        [143] = 31,
        [135] = 32,
        [127] = 33,
        [120] = 34,
        [113] = 35
    }

    return period_to_note_map[period] or 0
end

function ModControl:noteToPeriod(note)
    local note_to_period_map = {
        [0] = 856,
        [1] = 808,
        [2] = 762,
        [3] = 720,
        [4] = 678,
        [5] = 640,
        [6] = 604,
        [7] = 570,
        [8] = 538,
        [9] = 508,
        [10] = 480,
        [11] = 453,
        [12] = 428,
        [13] = 404,
        [14] = 381,
        [15] = 360,
        [16] = 339,
        [17] = 320,
        [18] = 302,
        [19] = 285,
        [20] = 269,
        [21] = 254,
        [22] = 240,
        [23] = 226,
        [24] = 214,
        [25] = 202,
        [26] = 190,
        [27] = 180,
        [28] = 170,
        [29] = 160,
        [30] = 151,
        [31] = 143,
        [32] = 135,
        [33] = 127,
        [34] = 120,
        [35] = 113
    }

    return note_to_period_map[note] or 0
end

function ModControl:isBitSet(number, bitIndex)
    local bitFlag = bit32.lshift(1, bitIndex)
    return bit32.band(number, bitFlag) == bitFlag
end

function ModControl:setBit(number, bitIndex, value)
    local bitFlag = bit32.lshift(1, bitIndex)
    if value then
        return bit32.bor(number, bitFlag) -- Set the bit to 1
    else
        return bit32.band(number, bit32.bnot(bitFlag)) -- Set the bit to 0
    end
end

return ModControl
