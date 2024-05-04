local Channel = {}
Channel.__index = Channel

local noteWidth = 10
local noteHeight = 5
local noteSpacing = 2

function Channel.new(channelNumber, x, y)
    local self = setmetatable({}, Channel)
    self.x = x
    self.y = y
    self.channelNumber = channelNumber
    self.notes = {}
    for j = 1, 16 do
        table.insert(self.notes, {
            noteIndex = 0,
            octave = 0,
            instrument = 0,
            volume = 9,
            effect = 0
        })
    end
    self.selectedNote = 1
    self.selectedField = "noteIndex"
    return self
end

function Channel:draw()
    -- Draw the channel interface with notes at position (x, y)
    for i, note in ipairs(self.notes) do
        trace("index: "..i)
        trace("noteIndex: "..note.noteIndex)
        trace("x: "..self.x)
        trace("y: "..self.y)
        trace("noteHeight: "..noteHeight)
        trace("noteSpacing: "..noteSpacing)


        local noteX = self.x
        local noteY = self.y + (i - 1) * (noteHeight + noteSpacing)
        -- draw a rectangle for the background of the note
        rectfill(noteX, noteY, noteWidth, noteHeight, 2)
        -- draw the note text (you can customize the text format)
        local noteText = string.format("%X%X%X%X%X", note.noteIndex, note.octave, note.instrument, note.volume, note.effect)
        print(noteText, noteX + 1, noteY + 1, 1)
        
        if i == self.selectedNote then
            -- draw a rectangle around the selected note
            rect(noteX, noteY, noteWidth, noteHeight, 1)
        end
    end
end

function Channel:key(key_code, ctrl_pressed, shift_pressed)
    -- Handle user input for editing notes, adding notes, and deleting notes
    if key_code == KEYCODE.KEY_UP then
        self.selectedNote = math.max(1, self.selectedNote - 1)
    elseif key_code == KEYCODE.KEY_DOWN then
        self.selectedNote = math.min(#self.notes, self.selectedNote + 1)
    elseif key_code == KEYCODE.KEY_LEFT or key_code == KEYCODE.KEY_RIGHT then
        local note = self.notes[self.selectedNote]
        local delta = (key_code == KEYCODE.KEY_RIGHT) and 1 or -1
        if self.selectedField == "noteIndex" then
            note.noteIndex = (note.noteIndex + delta) % 16
        elseif self.selectedField == "octave" then
            note.octave = (note.octave + delta) % 8
        elseif self.selectedField == "instrument" then
            note.instrument = (note.instrument + delta) % 10
        elseif self.selectedField == "volume" then
            note.volume = (note.volume + delta) % 10
        elseif self.selectedField == "effect" then
            note.effect = (note.effect + delta) % 10
        end
    elseif key_code == KEYCODE.KEY_A and ctrl_pressed then
        self:addNote()
    elseif key_code == KEYCODE.KEY_D and ctrl_pressed then
        self:deleteNote()
    end
end

function Channel:addNote()
    -- Add a new note to the channel
    local newNote = {
        noteIndex = 0,
        octave = 0,
        instrument = 0,
        volume = 9,
        effect = 0
    }
    table.insert(self.notes, self.selectedNote + 1, newNote)
    self.selectedNote = math.min(#self.notes, self.selectedNote + 1)
end

function Channel:deleteNote()
    -- Delete the selected note from the channel
    if #self.notes > 1 then
        table.remove(self.notes, self.selectedNote)
        self.selectedNote = math.min(#self.notes, self.selectedNote)
    else
        -- If there's only one note, reset it to default values
        local note = self.notes[1]
        note.noteIndex = 0
        note.octave = 0
        note.instrument = 0
        note.volume = 9
        note.effect = 0
    end
end

return Channel
