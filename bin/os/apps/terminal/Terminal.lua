local Terminal = {}
Terminal.__index = Terminal

function Terminal.new(editCallback, spriteEditCallback, loadCartCallback,
                      saveCartCallback, importCodeCallback)
    local self = setmetatable({}, Terminal)

    self.input = ""
    self.cursorBlink = 0
    self.cursorVisible = true
    self.history = {}
    self.commandsHistory = {}
    self.commandsHistoryIndex = 1
    self.linesOnScreen = 120 / 8
    self.cursorPos = 0

    self.editCallback = editCallback
    self.spriteEditCallback = spriteEditCallback
    self.loadCartCallback = loadCartCallback
    self.saveCartCallback = saveCartCallback
    self.importCodeCallback = importCodeCallback

    cls()
    trace("Terminal started")
    self:printLn("", 2)
    self:printLn("nibble-8", 2)
    self:printLn("version 0.0.1", 2)
    self:printLn("", 2)
    self:printLn("type help for help", 2)
    self:printLn("", 2)

    return self;
end

function Terminal:printLn(line, color)
    item = {text = line, color = color}
    table.insert(self.history, item)
    if #self.history > self.linesOnScreen - 1 then
        table.remove(self.history, 1)
    end
end

function Terminal:printHistory()
    for i = 1, #self.history, 1 do
        print(self.history[i].text, 0, (i - 1) * 8, self.history[i].color)
    end
end

function Terminal:printLs(path)
    local files
    if path then
        files = split(ls(path), "\n")
    else
        files = split(ls(), "\n")
    end
    for i = 1, #files, 1 do self:printLn(files[i], 2) end
end

function Terminal:loadCart(path)
    if sub(path, -4) ~= ".n8" then path = path .. ".n8" end
    local errorMsg = self.loadCartCallback(path)
    if errorMsg ~= nil then
        self:printLn(errorMsg, 2)
    else
        local cartname
        for i = #path, 1, -1 do
            if sub(path, i, i) == "/" then
                cartname = sub(path, i + 1)
                break
            end
        end

        self:printLn("cart "..cartname.." loaded sucessfully", 2)
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

    if command[1] == "cls" then
        self.history = {}
    elseif command[1] == "cd" then
        cd(command[2])
    elseif command[1] == "ls" then
        self:printLs(command[2])
    elseif command[1] == "load" then
        self:loadCart(command[2])
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
        self:printLn("load <file> - load a cart", 2)
        self:printLn("save <file> - save a cart", 2)
        self:printLn("import <file> - import spritesheet png", 2)
        self:printLn("export <file> - export spritesheet png", 2)
        self:printLn("cd - change directory", 2)
        self:printLn("ls - list files", 2)
        self:printLn("cls - clear screen", 2)
        self:printLn("exit - quit terminal", 2)
        self:printLn("", 2)
        self:printLn("press esc to toggle editor view", 2)
    elseif command[1] == "exit" then
        self:printLn("Bye Bye!", 2)
    else
        self:printLn("invalid command: " .. command, 1)
        self:printLn("type 'help' for a list of commands", 1)
    end

    table.insert(self.commandsHistory, self.input)
    self.commandsHistoryIndex = #self.commandsHistory + 1

    self.input = ""
    self.cursorPos = 0
end

function Terminal:key(key_code, ctrl_pressed, shift_pressed)
    local inputText = UTILS.handle_text_input(key_code, ctrl_pressed,
                                              shift_pressed)
    local leftPart = ""
    local rightPart = ""

    if inputText ~= "" then
        leftPart = sub(self.input, 1, self.cursorPos)
        rightPart = sub(self.input, self.cursorPos + 1)

        self.input = leftPart .. inputText .. rightPart
        self.cursorPos = self.cursorPos + 1
    end

    if key_code == KEYCODE.KEY_BACKSPACE then
        if self.cursorPos <= #self.input and self.cursorPos > 0 then
            leftPart = sub(self.input, 1, self.cursorPos - 1)
            rightPart = sub(self.input, self.cursorPos + 1)

            self.input = leftPart .. rightPart
            self.cursorPos = self.cursorPos - 1
        end
    end

    if key_code == KEYCODE.KEY_DELETE then
        if self.cursorPos < #self.input and self.cursorPos >= 0 then
            leftPart = sub(self.input, 1, self.cursorPos)
            rightPart = sub(self.input, self.cursorPos + 2)

            self.input = leftPart .. rightPart
        end
    end

    if key_code == KEYCODE.KEY_RETURN then self:executeInput() end

    -- previous command from history
    if key_code == KEYCODE.KEY_UP then
        if self.commandsHistoryIndex > 1 then
            self.commandsHistoryIndex = self.commandsHistoryIndex - 1
            self.input = self.commandsHistory[self.commandsHistoryIndex]
            self.cursorPos = #self.input
        end
    end

    -- next command from history
    if key_code == KEYCODE.KEY_DOWN then
        if self.commandsHistoryIndex < #self.commandsHistory then
            self.commandsHistoryIndex = self.commandsHistoryIndex + 1
            self.input = self.commandsHistory[self.commandsHistoryIndex]
            self.cursorPos = #self.input
        else
            self.commandsHistoryIndex = #self.commandsHistory + 1
            self.input = ""
            self.cursorPos = 0
        end
    end

    if key_code == KEYCODE.KEY_LEFT then
        self.cursorPos = math.max(self.cursorPos - 1, 0)
    end

    if key_code == KEYCODE.KEY_RIGHT then
        self.cursorPos = math.min(self.cursorPos + 1, #self.input)
    end
end

function Terminal:keyup(key_code, ctrl_pressed, shift_pressed) end

function Terminal:init() end

function Terminal:update()
    self.cursorBlink = self.cursorBlink + 1
    if self.cursorBlink % 20 == 0 then
        self.cursorVisible = not self.cursorVisible
    end
end

function Terminal:draw()
    cls()
    self:printHistory()
    if self.cursorVisible then
        print(chr(16), (self.cursorPos + 2) * 4, #self.history * 8, 2)
    end
    print("> " .. self.input, 0, #self.history * 8, 3)
end

return Terminal
