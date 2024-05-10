local Terminal = {}
Terminal.__index = Terminal

local ModControl = require("os/apps/sfx/ModControl")

function Terminal.new(editCallback, spriteEditCallback, loadCartCallback,
                      saveCartCallback, importCodeCallback)
    local self = setmetatable({}, Terminal)

    self.logo = {
        0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 3, 2, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 3, 3, 0, 0, 0, 3, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3,
        0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,
        0, 2, 3, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0,
        3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 3,
        0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 0, 3, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3,
        3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0,
        0, 0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 0, 0, 0, 0, 0,
        2, 2, 2, 0, 3, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 3, 3, 0, 0, 3, 3,
        0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0,
        0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 0, 0, 3, 0, 0, 0, 0, 0, 2,
        2, 2, 2, 3, 2, 3, 2, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 3, 3, 0, 0, 3, 3, 0,
        0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 2, 2,
        3, 2, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0,
        3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 0, 3,
        0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 0, 0, 0, 3, 0, 0, 2, 2, 2, 0, 2, 3,
        3, 3, 3, 3, 3, 0, 0, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3,
        3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 0,
        0, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 0, 0, 3, 2, 2, 2, 2, 2, 2, 0, 0, 2,
        3, 3, 3, 0, 0, 0, 3, 0, 0, 3, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 0, 0,
        0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0,
        0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 2, 2, 2, 2, 2, 2, 3, 0, 0,
        0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 2, 2, 2, 0, 2, 2, 3, 3, 2, 0,
        0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 2, 0, 0, 2, 3, 0, 2, 3, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
    self.logoWidth = 71
    self.logoHeight = 13
    self.input = ""
    self.cursor = {x = 0, y = 0, visible = false, blink = 0, pos = 0}
    self.history = {}
    self.commandsHistory = {}
    self.commandsHistoryIndex = 1
    self.linesOnScreen = 120 / 6 - 1

    self.editCallback = editCallback
    self.spriteEditCallback = spriteEditCallback
    self.loadCartCallback = loadCartCallback
    self.saveCartCallback = saveCartCallback
    self.importCodeCallback = importCodeCallback

    self.messageQueue = {}
    self.queueTimer = 0

    cls()
    local sample = 1
    local volume = 64
    -- local notes = {11, 20, 15, 22}
    local notes = {28, 30, 31, 33}
    self:queueImage(self.logo, self.logoWidth, self.logoHeight, 0.025,
                    {note = notes[1], sample = sample, volume = volume})
    self:queueMessage("nibble8 v.0.0.1", 2, 0.07,
                      {note = notes[2], sample = sample, volume = volume})
    self:queueMessage("", 2, 0.03)
    self:queueMessage("type \f2help\f3 for help", 3, 0.05,
                      {note = notes[3], sample = sample, volume = volume})
    self:queueMessage("", 2, 0.07,
                      {note = notes[4], sample = sample, volume = volume})
                      self:queueMessage("", 2, 0.07,
                      {note = -1, sample = sample, volume = volume})
    return self;
end

function Terminal:queueImage(image, width, height, delay, sound)
    table.insert(self.messageQueue, {
        image = image,
        width = width,
        height = height,
        delay = delay,
        sound = sound
    })
end

function Terminal:queueMessage(text, color, delay, sound)
    table.insert(self.messageQueue,
                 {text = text, color = color, delay = delay, sound = sound})
end

function Terminal:printImage(image, width, height)
    local item = {image = image, width = width, height = height}
    table.insert(self.history, item)
    -- Calculate how many terminal lines the image occupies
    local imageLines = math.ceil(height / 8) -- Assuming each terminal line is 8 pixels tall
    for i = 1, imageLines - 1 do
        -- Insert empty placeholders for the extra lines the image occupies
        table.insert(self.history, {text = "", color = 0})
    end
    -- Adjust the history to keep within the terminal's line capacity
    while #self.history > self.linesOnScreen do table.remove(self.history, 1) end
end

function Terminal:getCursorY()
    -- Recalculate the cursor Y position based on the number of lines in the history
    local cursorY = 0
    for i = 1, #self.history do
        if self.history[i].image then
            -- If the item is an image, increment the Y cursor by the height of the image in terminal lines
            cursorY = cursorY + math.ceil(self.history[i].height / 8)
        else
            -- If the item is text, increment the Y cursor by 1
            cursorY = cursorY + 1
        end
    end

    return cursorY
end

function Terminal:printLn(line, color)
    local maxLineLength = 40
    local maxLinesOnScreen = self.linesOnScreen - 1
    local chunks = {}

    if line == nil then return end

    -- Split the line into individual lines based on newline characters
    local lines = {}
    for substr in line:gmatch("[^\n]+") do table.insert(lines, substr) end

    -- Process each line separately
    for _, singleLine in ipairs(lines) do
        -- Check if the line length exceeds the maximum allowed length
        if #singleLine > maxLineLength then
            -- Split the line into chunks of maxLineLength characters
            for i = 1, #singleLine, maxLineLength do
                local chunk = string.sub(singleLine, i, i + maxLineLength - 1)
                table.insert(chunks, chunk)
            end
        else
            -- If the line is short enough, add it as a single chunk
            table.insert(chunks, singleLine)
        end
    end

    -- Insert each chunk into the history table with the specified color
    for _, chunk in ipairs(chunks) do
        local item = {text = chunk, color = color}
        table.insert(self.history, item)

        -- Remove the oldest line if the history exceeds the maximum lines on screen
        if #self.history > maxLinesOnScreen then
            table.remove(self.history, 1)
        end
    end
end

function Terminal:drawImage(image, width, tx, ty)
    local x
    local y
    for i = 1, #image do
        pset((i - 1) % width + tx, flr((i - 1) / width) + tx, image[i])
    end
end

function Terminal:printHistory()
    local y = 0
    for _, item in ipairs(self.history) do
        if item.image then
            -- Draw each image at its respective starting line
            self:drawImage(item.image, item.width, 1, y)
            y = y + math.ceil(item.height / 6) - 1 -- Move Y cursor by the height of the image in terminal lines
        else
            -- Draw text items at their line position
            print(item.text, 1, y * 6, item.color)
            y = y + 1
        end
    end
end

function Terminal:printLs(path)
    local files
    if path then
        files = split(ls(path), "\n")
    else
        files = split(ls(), "\n")
    end

    local column_width = 18
    local max_filename_length = column_width - 1 -- Subtract 1 for the tilde (~) if needed

    local columns = 2
    local files_per_column = math.ceil(#files / columns)

    for i = 1, files_per_column do
        local line = "" -- Initialize the line as an empty string

        for j = 0, columns - 1 do
            local index = i + j * files_per_column
            if index <= #files and files[index] ~= "" then -- Ensure index is valid and not an empty line
                local filename = files[index]:sub(1, max_filename_length)
                if #files[index] > max_filename_length then
                    filename = filename .. "~"
                end
                line = line .. filename -- Append filename to line
                -- Add padding to make columns align
                if #filename < column_width then
                    line = line .. string.rep(" ", column_width - #filename)
                end
            end
        end
        if #line > 0 then
            self:printLn(line, 2) -- Print the complete line
        end
    end
end

function Terminal:loadCart(path)
    local errorMsg = self.loadCartCallback(path)
    if errorMsg ~= nil then
        self:printLn(errorMsg, 2)
    else
        local cartname = path
        for i = #path, 1, -1 do
            if sub(path, i, i) == "/" then
                cartname = sub(path, i + 1)
                break
            end
        end

        self:printLn("cart " .. cartname .. " loaded sucessfully", 2)
    end
end

function Terminal:saveCart(path)
    self:printLn("saving cart...", 2)
    if path then
        -- check extension
        if sub(path, -4) ~= ".n8" then path = path .. ".n8" end
        trace("path: " .. path)
        self.saveCartCallback(path)
    else
        trace("Saving current cart")
        self.saveCartCallback()
    end
end

function Terminal:export(path)
    if path then
        -- check extension
        if sub(path, -4) == ".png" then
            self:printLn("exporting spritesheet " .. path, 2)
            export_png(path)
            return
        elseif sub(path, -4) == ".lua" then
            self:printLn("exporting lua script " .. path, 2)
            export_lua(path)
        else
            self:printLn("error: specify png or lua extension", 2)
        end
    end
end

function Terminal:import(path)
    if path then
        -- check extension
        if sub(path, -4) == ".png" then
            self:printLn("importing spritesheet " .. path, 2)
            import_png(path)
            return
        elseif sub(path, -4) == ".lua" then
            self:printLn("importing lua script " .. path, 2)
            self.importCodeCallback(import_lua(path))
        else
            self:printLn("error: specify png or lua extension", 2)
        end

    end
end

function Terminal:parseArgs(command)
    local args = {}
    local arg = ""
    local in_quotes = false
    for i = 1, #command, 1 do
        local c = sub(command, i, i)
        if c == "\"" then
            in_quotes = not in_quotes
        elseif c == " " and not in_quotes then
            table.insert(args, arg)
            arg = ""
        else
            arg = arg .. c
        end
    end
    table.insert(args, arg)
    return args
end

function Terminal:executeInput()
    local command = self:parseArgs(self.input)
    self:printLn("> " .. self.input, 3)

    trace("command: \"" .. command[1] .. "\"")

    if command[1] == "" then
        return
    elseif command[1] == "cls" then
        self.history = {}
    elseif command[1] == "cd" then
        cd(command[2])
    elseif command[1] == "ls" then
        self:printLs(command[2])
    elseif command[1] == "load" then
        self:loadCart(command[2])
    elseif command[1] == "run" then
        runCart(command[2])
    elseif command[1] == "import" then
        self:import(command[2])
    elseif command[1] == "export" then
        self:export(command[2])
    elseif command[1] == "edit" then
        trace("Edit file: " .. command[2])
        -- trace(str(self.editCallback))
        self.editCallback(command[2])
    elseif command[1] == "spredit" then
        self.spriteEditCallback(command[2])
    elseif command[1] == "save" then
        trace(#command)
        if #command == 2 then
            self:saveCart(command[2])
        else
            self:saveCart()
        end
    elseif command[1] == "reboot" then
        reboot()
    elseif command[1] == "help" then
        self:printLn("", 1)
        self:printLn("commands", 3)
        self:printLn("", 1)
        self:printLn("\f2load <filename>\f3 - load a cart", 3)
        self:printLn("\f2save <filename>\f3 - save a cart", 3)
        self:printLn("\f2run (or ctrl+r)\f3 - run", 3)
        self:printLn("\f2import <file>\f3 - import spritesheet png", 3)
        self:printLn("\f2export <file>\f3 - export spritesheet png", 3)
        self:printLn("\f2cd\f3 - change directory", 3)
        self:printLn("\f2ls\f3 - list files", 3)
        self:printLn("\f2cls\f3 - clear screen", 3)
        self:printLn("\f2exit\f3 - quit terminal", 3)
        self:printLn("", 2)
        self:printLn("press \f2esc\f3 to toggle editor view", 3)
    elseif command[1] == "exit" then
        exit()
    else
        self:printLn("invalid command: \f2" .. command[1] .. "\f1", 1)
        self:printLn("type \f2help\f3 for a list of commands", 3)
    end

    table.insert(self.commandsHistory, self.input)
    self.commandsHistoryIndex = #self.commandsHistory + 1

    self.input = ""
    self.cursor.pos = 0
end

function Terminal:key(key_code, ctrl_pressed, shift_pressed)
    if #self.messageQueue > 0 then return end

    local inputText = UTILS.handle_text_input(key_code, ctrl_pressed,
                                              shift_pressed)
    local leftPart = ""
    local rightPart = ""

    if inputText ~= "" then
        leftPart = sub(self.input, 1, self.cursor.pos)
        rightPart = sub(self.input, self.cursor.pos + 1)

        self.input = leftPart .. inputText .. rightPart
        self.cursor.pos = self.cursor.pos + 1
    end

    if key_code == KEYCODE.KEY_BACKSPACE then
        if self.cursor.pos <= #self.input and self.cursor.pos > 0 then
            leftPart = sub(self.input, 1, self.cursor.pos - 1)
            rightPart = sub(self.input, self.cursor.pos + 1)

            self.input = leftPart .. rightPart
            self.cursor.pos = self.cursor.pos - 1
        end
    end

    if key_code == KEYCODE.KEY_DELETE then
        if self.cursor.pos < #self.input and self.cursor.pos >= 0 then
            leftPart = sub(self.input, 1, self.cursor.pos)
            rightPart = sub(self.input, self.cursor.pos + 2)

            self.input = leftPart .. rightPart
        end
    end

    if key_code == KEYCODE.KEY_RETURN then self:executeInput() end

    -- previous command from history
    if key_code == KEYCODE.KEY_UP then
        if self.commandsHistoryIndex > 1 then
            self.commandsHistoryIndex = self.commandsHistoryIndex - 1
            self.input = self.commandsHistory[self.commandsHistoryIndex]
            self.cursor.pos = #self.input
        end
    end

    -- next command from history
    if key_code == KEYCODE.KEY_DOWN then
        if self.commandsHistoryIndex < #self.commandsHistory then
            self.commandsHistoryIndex = self.commandsHistoryIndex + 1
            self.input = self.commandsHistory[self.commandsHistoryIndex]
            self.cursor.pos = #self.input
        else
            self.commandsHistoryIndex = #self.commandsHistory + 1
            self.input = ""
            self.cursor.pos = 0
        end
    end

    if key_code == KEYCODE.KEY_LEFT then
        self.cursor.pos = math.max(self.cursor.pos - 1, 0)
    end

    if key_code == KEYCODE.KEY_RIGHT then
        self.cursor.pos = math.min(self.cursor.pos + 1, #self.input)
    end
end

function Terminal:keyup(key_code, ctrl_pressed, shift_pressed) end

function Terminal:init() end

function Terminal:update()
    if #self.messageQueue > 0 then
        self.queueTimer = self.queueTimer + 0.02 -- Assuming 50 FPS, adjust based on actual frame rate
        local msg = self.messageQueue[1]
        if self.queueTimer >= msg.delay then
            if msg.image then
                self:printImage(msg.image, msg.width, msg.height)
            elseif msg.text then
                self:printLn(msg.text, msg.color)
            end

            if msg.sound then
                if msg.sound.note == -1 then
                    ModControl:stopNote()
                else
                    ModControl:playNote(msg.sound.note, msg.sound.sample,
                                    msg.sound.volume)
                end
            end

            table.remove(self.messageQueue, 1)
            self.queueTimer = 0
        end
    else
        self.cursor.blink = self.cursor.blink + 1
    end
end

function Terminal:draw()
    cls()
    self:printHistory()
    if #self.messageQueue > 0 then return end

    self.cursor.x = (self.cursor.pos + 2) * 4 + 1
    self.cursor.y = self:getCursorY() * 6

    if self.cursor.blink % 20 == 0 then
        self.cursor.visible = not self.cursor.visible
    end

    if self.cursor.visible then
        -- Draw cursor if it's time to blink it on
        print(chr(16), self.cursor.x, self.cursor.y, 1)
    end
    -- Draw the current input line
    print("> " .. self.input, 1, self.cursor.y, 3)
end

return Terminal
