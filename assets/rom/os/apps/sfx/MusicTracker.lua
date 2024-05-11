local MusicTracker = {}
MusicTracker.__index = MusicTracker

local ModControl = require("os/apps/sfx/ModControl")
local UIManager = require("ui/UIManager")
local FancyStepper = require("os/apps/sfx/FancyStepper")
local ListSelector = require("os/apps/sfx/ListSelector")
-- local KeyboardUtils = require("os/apps/synth/KeyboardUtils")
local UIUtils = require("ui/UIUtils")
local NUM_CHANNELS = 4
-- Define the width of each column in the pattern editor
local columnWidths = {
    3, -- Note
    1, -- Instrument 1
    1, -- Instrument 2
    1, -- Command
    1, -- Param
    1, -- Param
    1 -- Channel separator
}

local uiManager
local posSelector
local patternSelector
local bpmSelector
local lengthSelector
local volumeSelector
local instrumentSelector

function MusicTracker.new(x, y)
    local self = setmetatable({}, MusicTracker)
    self.x = x
    self.y = y
    self.cursorRow = 1 -- Start at the first row
    self.cursorColumn = 1
    self.visibleLines = 15
    self.topVisibleLine = 1
    self.numColumns = 6 * NUM_CHANNELS
    self.cursorX = self.x + 2
    self.playing = false
    self.editMode = false
    -- self.currentInstrument = 1

    self.columnWidths = {}
    for i = 1, NUM_CHANNELS do
        for _, width in ipairs(columnWidths) do
            table.insert(self.columnWidths, width)
        end
    end
    self.numColumns = #self.columnWidths

    ModControl:init()

    uiManager = UIManager.new()
    -- First row
    patternSelector = FancyStepper.new(x + 2, y + 1, 0, 99, "pattern",
                                       function(value)
        trace("pattern: " .. str(value))
        self:invalidateTrack()
    end)
    --bpmSelector = FancyStepper.new(x + 52, y + 1, 0, 255, "bpm", function(value)
        --trace("BPM: " .. str(value))
    --end)

    -- Second row
    --volumeSelector = FancyStepper.new(x + 2, y + 12, 0, 64, "volume", function(
        --value) self.volume = volumeSelector.value end)

    local instruments = ModControl:getSamples()
    instrumentSelector = ListSelector.new(x + 50, y + 1, "instrument",
                                          instruments, 15, function(value)
        -- trace("instrument: " .. str(value))
        self.currentInstrument = value
    end)

    uiManager:addElement(posSelector)
    uiManager:addElement(patternSelector)
    --uiManager:addElement(bpmSelector)
    -- uiManager:addElement(lengthSelector)
    --uiManager:addElement(volumeSelector)
    uiManager:addElement(instrumentSelector)

    self.lines = ""
    self.track = {}
    self:init()

    return self
end

function MusicTracker:init()
    patternSelector.value = patternSelector.minValue
    instrumentSelector.selectedIndex = 1
    --bpmSelector.value = 125
    --volumeSelector.value = 0x40
    self.currentInstrument = 1
    --self.volume = volumeSelector.value

    self:invalidateTrack()
end

function MusicTracker:invalidateTrack()
    self.track = ModControl:getPattern(patternSelector.value)
    self:updateText()
end

function MusicTracker:updateText()
    self.lines = ""
    for i = self.topVisibleLine, self.topVisibleLine + self.visibleLines - 1 do
        self.lines = self.lines .. self:getLineText(i, true) .. "\n"
    end
end

function MusicTracker:getLineText(i, highlight)
    local line = ""

    if highlight then
        if (i - 1) % 4 == 0 then
            line = "\f2" .. string.format("%02d", i - 1) .. " "
        else
            line = "\f1" .. string.format("%02d", i - 1) .. " "
        end
    else
        line = string.format("%02d", i - 1) .. " "
    end

    for j = 1, NUM_CHANNELS do
        local index = (i - 1) * NUM_CHANNELS + j
        local note = self.track[index]
        local noteName
        local octave
        local instrument = string.format("%02x", note[3])
        local command = string.format("%x", note[4])
        local param = string.format("%02x", note[5])
        local noteText = ""

        if note[1] > 0 and note[3] > 0 then
            noteName = ModControl.NOTES[note[1]]
        else
            noteName = "--"
        end

        if note[2] > 0 and note[1] > 0 and note[3] > 0 then
            octave = string.format("%d", note[2])
        else
            octave = "-"
        end

        if highlight then
            noteText = noteText .. "\f3" .. noteName .. octave
            noteText = noteText .. "\f1" .. instrument
            noteText = noteText .. "\f2" .. command
            noteText = noteText .. "\f3" .. param
        else
            noteText = noteText .. noteName .. octave
            noteText = noteText .. instrument
            noteText = noteText .. command
            noteText = noteText .. param
        end

        line = line .. noteText .. " "
    end
    line = string.sub(line, 1, -2)
    return line
end

function MusicTracker:drawPanel()
    -- self:fancyPrint("pos", self.x + 2, self.y + 2, 3, 0)
    -- self:fancyRect(self.x + 15, self.y, 10, 8, 2, 3, 0)
    -- print("00", self.x + 17, self.y + 2, 3)
end

function MusicTracker:drawPatternEditor()
    local baseY = self.y + 20
    --local height = 70
    local height = 100
    palt(0, false)

    -- Draw the background
    rectfill(self.x + 2, baseY - 7, 156, height, 0)

    -- Draw the text
    print(self.lines, self.x + 4, baseY + 3, 1)

    -- Draw the cursor
    local cursorY = baseY + ((self.cursorRow - self.topVisibleLine) * 6)
    rectfill(self.x + 3, cursorY + 2, 153, 7, 2)
    -- draw second cursor
    if self.editMode and not self.playing then
        rectfill(self.cursorX + 13, cursorY + 2,
                 self.columnWidths[self.cursorColumn] * 4 + 1, 7, 3)
    end

    -- Draw the cursor text
    palt(1, true)
    print(self:getLineText(self.cursorRow, false), self.x + 4, cursorY + 3, 0, 1)
    palt(1, false)

    -- draw grid border with shadow
    UIUtils.fancyRect(self.x + 1, baseY - 8, 156, height, 2, 3)
    line(3, 119, 157, 119, 0) -- bottom shadow
    line(158, 118, 158, baseY - 6, 0) -- right shadow

    -- draw channels separators
    for i = 1, NUM_CHANNELS do
        local xPos = self.x + 2 + i * 36 - 25
        line(xPos - 1, baseY - 7, xPos - 1, 117, 0)
        line(xPos, baseY - 7, xPos, 117, 1)
        line(xPos + 1, baseY - 7, xPos + 1, 117, 0)
    end

    print("#", 6, baseY - 6, 3)

    for i = 1, NUM_CHANNELS do
        print(str(i), self.x + 2 + i * 36 - 8, baseY - 6, 3)
        -- print("s", self.x + 2 + i * 36 + 2, baseY - 6, 1)
        -- print("m", self.x + 2 + i * 36 + 7, baseY - 6, 1)
    end

    line(2, baseY, 156, baseY, 1) -- top

    palt(0, true)
end

function MusicTracker:key(key_code, ctrl_pressed, shift_pressed)
    local channel = flr(self.cursorColumn / 7)
    local line = self.cursorRow
    local pattern = patternSelector.value
    local sample = self.currentInstrument

    if not self.playing then
        self:checkCursorKeys(key_code)

        if self.editMode then
            self:editInput(key_code)
        else
            ModControl:handleKeyInput(self, key_code, pattern, line, channel,
                                      sample)
        end
    end

    -- trace(key_code)

    if key_code == KEYCODE.KEY_SPACE then
        if self.playing then
            self.playing = false
            music(-1)
        else
            self.editMode = not self.editMode
        end
    elseif key_code == KEYCODE.KEY_RETURN then
        self.playing = not self.playing
        if self.playing then
            trace("Play!")
            music(patternSelector.value, 1)
        else
            trace("Stop!")
            music(-1)
        end
    end
end

function MusicTracker:editInput(key_code)
    local channel,columnType,noteData
    local lineIndex = self.cursorRow - 1
    local pattern = patternSelector.value
    local sample = self.currentInstrument

    channel = flr((self.cursorColumn) / 7)
    columnType = (self.cursorColumn) % 7
    noteData = self.track[lineIndex * NUM_CHANNELS + channel + 1]
    
    -- Convert SDL2 key_code to character if it's a hex character (0-9, A-F)
    local char = UTILS.handle_text_input(key_code, false, false)
    if columnType == 1 then -- Note column
        ModControl:handleKeyInput(self, key_code, pattern, lineIndex, channel,
                                  sample)
    elseif char and noteData then
        local value = tonumber(char, 16)
        if value then
            if columnType == 2 or columnType == 3 then -- Instrument columns
                -- Assuming instruments are stored in noteData[3] in two nibbles
                local newInstrument

                if columnType == 2 then
                    newInstrument = (value * 16) + (noteData[3] % 16)
                else
                    newInstrument = (noteData[3] - noteData[3] % 16) + value
                end

                if newInstrument > 32 then newInstrument = 32 end

                ModControl:setSample(pattern, lineIndex, channel, newInstrument)
                self:invalidateTrack()
                self:moveCursorV(1)
            elseif columnType == 4 then -- Command column
                ModControl:setCommandAndParams(pattern, lineIndex, channel, value, noteData[5])
                self:invalidateTrack()
                self:moveCursorV(1)
            elseif columnType == 5 or columnType == 6 then -- Param columns
                -- Assuming params are stored in noteData[5] as two hex digits
                local newParams

                if columnType == 5 then
                    newParams = (value * 16) + (noteData[5] % 16)
                else
                    newParams = (noteData[5] - noteData[5] % 16) + value
                end

                ModControl:setCommandAndParams(pattern, lineIndex, channel, noteData[4], newParams)
                self:invalidateTrack()
                self:moveCursorV(1)
            end
        end
    end
end

function MusicTracker:checkCursorKeys(key_code)
    local rowChanged = false
    local cursorMoved = false

    if (key_code == KEYCODE.KEY_UP) and (not ctrl_pressed) then
        self:moveCursorV(-1)
    elseif (key_code == KEYCODE.KEY_DOWN) and (not ctrl_pressed) then
        self:moveCursorV(1)
    elseif key_code == KEYCODE.KEY_PGUP or
        (key_code == KEYCODE.KEY_UP and ctrl_pressed) then
        self:moveCursorV(-self.visibleLines)
    elseif key_code == KEYCODE.KEY_PGDOWN or
        (key_code == KEYCODE.KEY_DOWN and ctrl_pressed) then
        self:moveCursorV(self.visibleLines)
    elseif key_code == KEYCODE.KEY_LEFT then
        -- Move cursor left, skipping separators
        if self.cursorColumn > 1 then
            self.cursorColumn = self.cursorColumn - 1
            -- Check if it's a separator and skip it
            if self.cursorColumn % 7 == 0 then -- Assuming separator is every 7th column
                self.cursorColumn = self.cursorColumn - 1
            end
            trace(self.cursorColumn)
        else
            self.cursorColumn = #self.columnWidths -- Wrap to last column, avoiding separators
            if self.cursorColumn % 7 == 0 then
                self.cursorColumn = self.cursorColumn - 1
            end
        end
        cursorMoved = true
    elseif key_code == KEYCODE.KEY_RIGHT then
        -- Move cursor right, skipping separators
        if self.cursorColumn < #self.columnWidths - 1 then
            self.cursorColumn = self.cursorColumn + 1
            -- Check if it's a separator and skip it
            if self.cursorColumn % 7 == 0 then -- Assuming separator is every 7th column
                self.cursorColumn = self.cursorColumn + 1
            end
            trace(self.cursorColumn)
        else
            self.cursorColumn = 1 -- Wrap to first column
        end
        cursorMoved = true
    end

    if cursorMoved then self:updateCursorX() end
end

function MusicTracker:moveCursorV(offset)
    local rowChanged = false
    local cursorMoved = false

    -- Calculate the new cursor row
    local newCursorRow = self.cursorRow + offset

    -- Check if the new cursor row exceeds the limits
    if newCursorRow < 1 then
        -- Wraparound to the bottom if the new cursor row is less than 1
        self.cursorRow = 64
        rowChanged = true
    elseif newCursorRow > 64 then
        -- Wraparound to the top if the new cursor row is greater than 64
        self.cursorRow = 1
        rowChanged = true
    else
        -- Update the cursor row if within the limits
        self.cursorRow = newCursorRow
        rowChanged = true
    end

    -- Ensure the cursor remains within the visible lines
    self:ensureCursorVisible()

    if rowChanged then self:updateText() end
end

function MusicTracker:keyup(key_code, ctrl_pressed, shift_pressed)
    ModControl:checkKeyUp(self, key_code)
end

function MusicTracker:update()
    self.playing = ModControl:isPlaying()
    if self.playing then
        local newRow = ModControl:getCurrentLine() + 1
        -- local pattern = ModControl:getCurrentPattern()
        -- if pattern ~= patternSelector.value then
        --    patternSelector.value = pattern
        --    self.lines = ModControl:getPattern(pattern)
        --    self:updateText()
        -- end
        if newRow ~= self.cursorRow and newRow <= 64 then
            self.cursorRow = newRow
            self:updateText()
            self:ensureCursorVisible()
        end
    end

    uiManager:update()
    ModControl:checkKeyUp()
end

function MusicTracker:updateCursorX()
    local x = self.x + 2 -- Start x position of the first column
    for i = 1, self.cursorColumn - 1 do x = x + self.columnWidths[i] * 4 end
    self.cursorX = x
end

function MusicTracker:ensureCursorVisible()
    if self.cursorRow < self.topVisibleLine then
        self.topVisibleLine = self.cursorRow
    elseif self.cursorRow > self.topVisibleLine + self.visibleLines - 1 then
        self.topVisibleLine = self.cursorRow - self.visibleLines + 1
    end
    -- Ensure topVisibleLine stays within valid bounds
    self.topVisibleLine = math.max(1, math.min(self.topVisibleLine,
                                               64 - self.visibleLines + 1))
end

function MusicTracker:drawPost()
    local msg = nil
    if self.playing then
        msg = "playing"
    elseif self.editMode then
        msg = "edit mode"
    end

    if msg then print(msg, 160 - #msg * 4, 1, 0) end
end

function MusicTracker:draw()
    cls(1)
    self:drawPanel()
    self:drawPatternEditor()
    --self:drawFakeBeat(self.x + 2, self.y + 23)
    --self:drawFakeSpectrum(self.x + 14, self.y + 23, 1)
    --self:drawFakeSpectrum(self.x + 50, self.y + 23, 1)
    --self:drawFakeSpectrum(self.x + 86, self.y + 23, 1)
    --self:drawFakeSpectrum(self.x + 122, self.y + 23, 1)
    uiManager:draw()
end

function MusicTracker:drawFakeSpectrum(sx, sy, type)
    local w = 35
    local h = 17
    local px = sx
    local py = sy + h / 2

    -- rect(63, 55, 34, 18, 1)
    rectfill(sx, sy, w, h, 0)

    for x = 0, w - 1 do
        local y
        local xp = x + t() * 50
        -- y=sin(xp/10)*rnd(1)*8
        y = rnd(1) * 16 + 1
        -- pset(sx+x,sy+y,2)
        line(px, py, sx + x, sy + y, 2)
        px = sx + x
        py = sy + y
    end

    rect(sx, sy, w, h, 0)
end

function MusicTracker:drawFakeBeat(sx, sy)
    local w = 11
    local h = 17
    local v = sin(t() * 2.5)
    local hn = flr((h - 1) * v)

    rectfill(sx, sy, w, h, 0)
    rectfill(sx + 1, sy + h - hn - 1, w - 2, hn, 2)
end

function MusicTracker:mousep(x, y, button) uiManager:mousepressed(x, y, button) end
function MusicTracker:mouser(x, y, button) uiManager:mousereleased(x, y, button) end
function MusicTracker:mousem(x, y) uiManager:mousemoved(x, y) end

return MusicTracker
