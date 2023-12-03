local MusicTracker = {}
MusicTracker.__index = MusicTracker

local KeyboardUtils = require("os/apps/synth/KeyboardUtils")
local NUM_CHANNELS = 4
local NUM_ROWS = 16
local COLUMN_WIDTH = 30
local COLUMN_WIDTHS = {
    2 * 4, -- 2 characters for Note
    4, -- 1 character for Octave
    2 * 4, -- 2 characters for Instrument
    4, -- 1 character for Volume
    1 * 4 -- 2 characters for Effect
}

function MusicTracker.new(x, y)
    local self = setmetatable({}, MusicTracker)
    self.x = x
    self.y = y

    self.channels = {}

    self.columns = {}
    self.columns[1] = 2
    self.columns[2] = 1
    self.columns[3] = 2
    self.columns[4] = 1
    self.columns[5] = 1

    self.channels[1] = {
        {note = 1, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 3, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 5, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 6, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 8, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 10, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 12, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 1, octave = "3", instrument = "01", volume = "2", effect = "1"},
        {note = 1, octave = "3", instrument = "01", volume = "2", effect = "1"},
        {note = 12, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 10, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 8, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 6, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 5, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 3, octave = "2", instrument = "01", volume = "2", effect = "1"},
        {note = 1, octave = "2", instrument = "01", volume = "2", effect = "1"}
    }

    for i = 1, #self.channels[1] do set_note(0, i - 1, 1, 2, 7, 3) end

    for i = 2, NUM_CHANNELS do
        self.channels[i] = {}
        for j = 1, NUM_ROWS do
            self.channels[i][j] = {
                -- note = "..",
                -- octave = ".",
                -- instrument = "..",
                -- volume = ".",
                -- effect = "."
                note = 2,
                octave = "2",
                instrument = "01",
                volume = "2",
                effect = "1"
            }
        end
    end

    self.selectedChannel = 1
    self.selectedRow = 1
    self.selectedColumn = 1 -- For selecting within the [NOTE, OCTAVE, INSTRUMENT, VOLUME, EFFECT]

    self.instrumentDigitCount = 0
    self.isPlaying = false
    self.currentPlaybackRow = 1
    self.tempo = 360 -- beats per minute, adjust as needed
    self.playbackStartTime = 0

    return self
end

function MusicTracker:startPlayback()
    self.isPlaying = true
    self.currentPlaybackRow = 1
    self.firstNotePlayed = false
    self.playbackStartTime = time() -- set the start time to the current time
    -- UPDATE_SYNTH(OSC, ATTACKTIME, DECAYTIME, SUSTAINAMPLITUDE, RELEASETIME, STARTAMPLITUDE)
    -- update_synth(6, 0, 0, 1, 1, 0)
end

function MusicTracker:stopPlayback()
    self.isPlaying = false
    self.currentPlaybackRow = 1 -- Reset the playback row when stopping
    note_off()
end

function MusicTracker:init() trace("init synth") end

function MusicTracker:update()
    if self.isPlaying then
        local elapsedTime = time() - self.playbackStartTime
        local timePerNote = 60 / self.tempo

        if not self.firstNotePlayed then
            -- Here we make sure that the note is not nil before playing the first note
            local firstNote =
                self.channels[self.selectedChannel][self.currentPlaybackRow]
                    .note
            if firstNote ~= nil then
                note_on(firstNote, tonumber(
                            self.channels[self.selectedChannel][self.currentPlaybackRow]
                                .octave))
            end
            self.firstNotePlayed = true
            self.playbackStartTime = time()
            return
        end

        if elapsedTime >= timePerNote then
            note_off() -- Stop previous note

            self.currentPlaybackRow = self.currentPlaybackRow + 1 -- Move to next row

            if self.currentPlaybackRow > NUM_ROWS then
                self:stopPlayback()
                return
            end

            -- Here we check if the note is not nil before playing the next note
            local nextNote =
                self.channels[self.selectedChannel][self.currentPlaybackRow]
                    .note
            if nextNote ~= nil then
                note_on(nextNote, tonumber(
                            self.channels[self.selectedChannel][self.currentPlaybackRow]
                                .octave))
            end

            self.playbackStartTime = time() -- Reset the start time
        end
    end
end

function MusicTracker:draw()
    rectfill(0, 7, 159, 113, 0) -- Background

    local rowHeight = 6
    local totalWidth = sumWidths(6, COLUMN_WIDTHS)
    local channelHeight = NUM_ROWS * rowHeight
    local padding = 2

    for channel, notes in ipairs(self.channels) do
        local channelStartX = self.x + (channel - 1) * (totalWidth + 4) +
                                  padding
        local channelEndX = channelStartX + totalWidth + padding - 1
        local channelStartY = self.y + padding
        local channelEndY = channelStartY + channelHeight + padding

        -- Draw border with rounded corners for each channel
        line(channelStartX + 1, channelStartY, channelEndX - 1, channelStartY, 2)
        line(channelStartX + 1, channelEndY, channelEndX - 1, channelEndY, 3)
        line(channelStartX, channelStartY + 1, channelStartX, channelEndY - 1, 2)
        line(channelEndX, channelStartY + 1, channelEndX, channelEndY - 1, 3)

        for row, note in ipairs(notes) do
            local yPos = channelStartY + (row - 1) * rowHeight + padding

            -- Drawing the selected cell background
            if channel == self.selectedChannel and row == self.selectedRow then
                rectfill(channelStartX +
                             sumWidths(self.selectedColumn, COLUMN_WIDTHS) +
                             padding - 1, yPos - 1,
                         COLUMN_WIDTHS[self.selectedColumn] + 1, rowHeight + 1,
                         3 -- Selected color
                )
            end

            if self.isPlaying and row == self.currentPlaybackRow then
                rectfill(channelStartX +
                             sumWidths(self.selectedColumn, COLUMN_WIDTHS) +
                             padding - 1, yPos - 1, 32, rowHeight + 1, 1 -- Selected color
                )
            end

            -- Function to determine text color based on selected state and if the note data is empty
            local function getTextColor(column, data, defaultColor)
                if (channel == self.selectedChannel and row == self.selectedRow and
                    column == self.selectedColumn) then
                    return 0
                elseif self.isPlaying and row == self.currentPlaybackRow then
                    return 0
                elseif not data or data == ".." or data == "." then
                    return 1
                else
                    return defaultColor
                end
            end

            -- Drawing the note data with an additional padding inside the channel and applying color based on selection or emptiness
            print(KeyboardUtils.noteNames[note.note] or "..",
                  channelStartX + padding, yPos,
                  getTextColor(1, KeyboardUtils.noteNames[note.note], 3))
            print(note.octave or ".",
                  channelStartX + sumWidths(2, COLUMN_WIDTHS) + padding, yPos,
                  getTextColor(2, note.octave, 2))
            print(note.instrument or "..",
                  channelStartX + sumWidths(3, COLUMN_WIDTHS) + padding, yPos,
                  getTextColor(3, note.instrument, 3))
            print(note.volume or ".",
                  channelStartX + sumWidths(4, COLUMN_WIDTHS) + padding, yPos,
                  getTextColor(4, note.volume, 2))
            print(note.effect or ".",
                  channelStartX + sumWidths(5, COLUMN_WIDTHS) + padding, yPos,
                  getTextColor(5, note.effect, 1))
        end
    end
end

function sumWidths(column, widths)
    local total = 0
    for i = 1, column - 1 do
        total = total + widths[i] -- +1 for padding between columns
        if i > 1 then total = total + 1 end
    end
    return total
end

function MusicTracker:key(key_code, ctrl_pressed, shift_pressed)
    -- Navigation
    if key_code == KEYCODE.KEY_UP and self.selectedRow > 1 then
        self.selectedRow = self.selectedRow - 1
    elseif key_code == KEYCODE.KEY_DOWN and self.selectedRow < NUM_ROWS then
        self.selectedRow = self.selectedRow + 1
    elseif key_code == KEYCODE.KEY_LEFT then
        if self.selectedColumn > 1 then
            self.selectedColumn = self.selectedColumn - 1
        elseif self.selectedChannel > 1 then
            self.selectedChannel = self.selectedChannel - 1
            self.selectedColumn = #COLUMN_WIDTHS
        end
    elseif key_code == KEYCODE.KEY_RIGHT then
        if self.selectedColumn < #COLUMN_WIDTHS then
            self.selectedColumn = self.selectedColumn + 1
        elseif self.selectedChannel < #self.channels then
            self.selectedChannel = self.selectedChannel + 1
            self.selectedColumn = 1
        end
    elseif key_code == KEYCODE.KEY_SPACE then
        if self.isPlaying then
            self:stopPlayback()
        else
            self:startPlayback()
        end
    end

    if key_code == KEYCODE.KEY_BACKSPACE then
        self.channels[self.selectedChannel][self.selectedRow].note = nil
        self.channels[self.selectedChannel][self.selectedRow].octave = nil
        self.channels[self.selectedChannel][self.selectedRow].instrument = nil
        self.channels[self.selectedChannel][self.selectedRow].volume = nil
        self.channels[self.selectedChannel][self.selectedRow].effect = nil
    end

    -- Note insertion
    if key_code >= KEYCODE.KEY_A and key_code <= KEYCODE.KEY_Z then
        -- Convert keycode to corresponding letter and update the selected column
        local note = KeyboardUtils.keyCodeToPiano(key_code)
        if self.selectedColumn == 1 then
            self.channels[self.selectedChannel][self.selectedRow].note = note

            -- Start playback when the note is inserted
            local selectedNote =
                self.channels[self.selectedChannel][self.selectedRow].note
            local selectedOctave = tonumber(
                                       self.channels[self.selectedChannel][self.selectedRow]
                                           .octave)
            note_on(selectedNote, selectedOctave)
        end
    end

    -- Handle octave input (0-7)
    if self.selectedColumn == 2 and key_code >= KEYCODE.KEY_0 and key_code <=
        KEYCODE.KEY_7 then
        local octave = tonumber(UTILS.handle_text_input(key_code, ctrl_pressed,
                                                        shift_pressed)) -- Extract the number from the keycode
        self.channels[self.selectedChannel][self.selectedRow].octave = tostring(
                                                                           octave)
    end

    -- Handle instrument input (0-63)
    if self.selectedColumn == 3 then
        if key_code >= KEYCODE.KEY_0 and key_code <= KEYCODE.KEY_9 then
            local num = tonumber(UTILS.handle_text_input(key_code, ctrl_pressed,
                                                         shift_pressed)) -- Extract the number from the keycode
            local currentInstrument = tonumber(
                                          self.channels[self.selectedChannel][self.selectedRow]
                                              .instrument) or 0

            if self.instrumentDigitCount == 0 then
                trace("Reset Instrument")
                self.instrumentDigitCount = 0
                currentInstrument = 0
            end

            currentInstrument = (currentInstrument * 10 + num) -- This allows input like "23" for instrument 23
            self.channels[self.selectedChannel][self.selectedRow].instrument =
                string.format("%02d", currentInstrument)

            self.instrumentDigitCount = self.instrumentDigitCount + 1

            if self.instrumentDigitCount >= 2 then
                self.instrumentDigitCount = 0
            end
        else
            self.instrumentDigitCount = 0
        end
    end

    -- Handle volume input (0-7)
    if self.selectedColumn == 4 and key_code >= KEYCODE.KEY_0 and key_code <=
        KEYCODE.KEY_7 then
        local volume = tonumber(UTILS.handle_text_input(key_code, ctrl_pressed,
                                                        shift_pressed)) -- Extract the number from the keycode
        self.channels[self.selectedChannel][self.selectedRow].volume = tostring(
                                                                           volume)
    end

    -- Handle effect input (0-7)
    if self.selectedColumn == 5 and key_code >= KEYCODE.KEY_0 and key_code <=
        KEYCODE.KEY_7 then
        local effect = tonumber(UTILS.handle_text_input(key_code, ctrl_pressed,
                                                        shift_pressed)) -- Extract the number from the keycode
        self.channels[self.selectedChannel][self.selectedRow].effect = tostring(
                                                                           effect)
    end
end

function MusicTracker:keyup(key_code, ctrl_pressed, shift_pressed)
    -- Handle key release events, e.g., stopping a note playback.
end

return MusicTracker
