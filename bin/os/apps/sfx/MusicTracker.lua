local MusicTracker = {}
MusicTracker.__index = MusicTracker

local UIManager = require("ui/UIManager")
local FancyStepper = require("os/apps/sfx/FancyStepper")
local ListSelector = require("os/apps/sfx/ListSelector")
local KeyboardUtils = require("os/apps/synth/KeyboardUtils")
local UIUtils = require("ui/UIUtils")
local NUM_CHANNELS = 4
local NOTES = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"}
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
    self.visibleLines = 10
    self.topVisibleLine = 1
    self.numColumns = 6 * NUM_CHANNELS
    self.cursorX = self.x + 2

    self.columnWidths = {}
    for i = 1, NUM_CHANNELS do
        for _, width in ipairs(columnWidths) do
            table.insert(self.columnWidths, width)
        end
    end
    self.numColumns = #self.columnWidths

    uiManager = UIManager.new()
    -- First row
    posSelector = FancyStepper.new(x + 2, y + 1, 0, 63, "pos",
                                   self.positionChanged)
    patternSelector = FancyStepper.new(x + 36, y + 1, 0, 63, "pattern",
                                       self.patternChanged)
    bpmSelector = FancyStepper.new(x + 86, y + 1, 0, 255, "bpm", bpmChanged)
    bpmSelector.value = 125
    -- lengthSelector = FancyStepper.new(x + 124, y + 1, 0, 63, "len", function(
    -- value) trace("length: " .. str(value)) end)

    -- Second row
    volumeSelector = FancyStepper.new(x + 2, y + 12, 0, 40, "volume", function(
        value) trace("length: " .. str(value)) end)

    local instruments = {
        "0 sine", "1 square", "2 saw", "3 triangle", "4 noise", "5 pulse",
        "6 kick", "7 snare", "8 hat", "9 cymbal", "10 tom", "11 clap",
        "12 voice", "13 bass", "14 lead", "15 pad"
    }
    instrumentSelector = ListSelector.new(x + 48, y + 12, "instrument",
                                          instruments, 15, function(value)
        trace("instrument: " .. str(value))
    end)

    uiManager:addElement(posSelector)
    uiManager:addElement(patternSelector)
    uiManager:addElement(bpmSelector)
    -- uiManager:addElement(lengthSelector)
    uiManager:addElement(volumeSelector)
    uiManager:addElement(instrumentSelector)

    self.channels = {}
    self.track = {}
    self.lines = ""

    for i = 1, 64 do
        for j = 1, NUM_CHANNELS do
            -- note index, octave, instrument, command, param
            table.insert(self.track, {2, 2, 4, 10, 1})
        end
    end
    self:updateText()

    return self
end

function MusicTracker.positionChanged(value) trace("pos: " .. str(value)) end
function MusicTracker.patternChanged(value) trace("pattern: " .. str(value)) end
function MusicTracker.bpmChanged(value) trace("bpm: " .. str(value)) end

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
        local noteName = NOTES[note[1]]
        local octave = string.format("%d", note[2])
        local instrument = string.format("%02d", note[3])
        local command = string.format("%x", note[4])
        local param = string.format("%02d", note[5])
        local noteText = ""

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
    local baseY = self.y + 48
    palt(0, false)

    -- Draw the background
    rectfill(self.x + 2, baseY - 7, 156, 70, 0)

    -- Draw the text
    print(self.lines, self.x + 4, baseY + 3, 1)

    -- Draw the cursor
    local cursorY = baseY + ((self.cursorRow - self.topVisibleLine) * 6)
    rectfill(self.x + 3, cursorY + 2, 153, 7, 1)
    -- draw second cursor
    rectfill(self.cursorX + 13, cursorY + 2,
             self.columnWidths[self.cursorColumn] * 4 + 1, 7, 2)

    -- Draw the cursor text
    palt(1, true)
    print(self:getLineText(self.cursorRow, false), self.x + 4, cursorY + 3, 0, 1)
    palt(1, false)

    -- draw grid border with shadow
    UIUtils.fancyRect(self.x + 1, baseY - 8, 156, 70, 2, 3)
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
    local rowChanged = false
    local cursorMoved = false

    if (key_code == KEYCODE.KEY_UP) and (not ctrl_pressed) then
        self.cursorRow = max(1, self.cursorRow - 1)
        rowChanged = true
    elseif (key_code == KEYCODE.KEY_DOWN) and (not ctrl_pressed) then
        self.cursorRow = min(64, self.cursorRow + 1)
        rowChanged = true
    elseif key_code == KEYCODE.KEY_PGUP or
        (key_code == KEYCODE.KEY_UP and ctrl_pressed) then
        self.cursorRow = max(1, self.cursorRow - self.visibleLines)
        rowChanged = true
    elseif key_code == KEYCODE.KEY_PGDOWN or
        (key_code == KEYCODE.KEY_DOWN and ctrl_pressed) then
        self.cursorRow = min(64, self.cursorRow + self.visibleLines)
        rowChanged = true
    elseif key_code == KEYCODE.KEY_LEFT then
        -- Move cursor left, skipping separators
        if self.cursorColumn > 1 then
            self.cursorColumn = self.cursorColumn - 1
            -- Check if it's a separator and skip it
            if self.cursorColumn % 7 == 0 then -- Assuming separator is every 7th column
                self.cursorColumn = self.cursorColumn - 1
            end
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
        else
            self.cursorColumn = 1 -- Wrap to first column
        end
        cursorMoved = true
    end

    self:ensureCursorVisible()
    if rowChanged then self:updateText() end
    if cursorMoved then self:updateCursorX() end
end

function MusicTracker:update() uiManager:update() end

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

function MusicTracker:drawPost() end

function MusicTracker:draw()
    cls(1)
    self:drawPanel()
    self:drawPatternEditor()
    self:drawFakeBeat(self.x + 2, self.y + 23)
    self:drawFakeSpectrum(self.x + 14, self.y + 23, 1)
    self:drawFakeSpectrum(self.x + 50, self.y + 23, 1)
    self:drawFakeSpectrum(self.x + 86, self.y + 23, 1)
    self:drawFakeSpectrum(self.x + 122, self.y + 23, 1)
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
