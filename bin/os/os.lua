local Intro = require("os/Intro")
local TextEditor = require("os/apps/text_editor/TextEditor")
local Terminal = require("os/apps/terminal/Terminal")
local MapEditor = require("os/apps/map_editor/MapEditor")
local MusicTracker = require("os/apps/sfx/MusicTracker")
local Synth = require("os/apps/synth/Synth")

local currentWindow = nil

local textEditor = nil
local mapEditor = nil
local spriteEditor = require("os/apps/sprite_editor/SpriteEditor")
local musicTracker = nil
local synth = nil
local terminal = nil

local lastWindow = nil
local tabs = {}

local currentCartPath = nil

local mouse_coords = {x = 0, y = 0}

function _init()
    cls()
    textEditor = TextEditor.new("", 1, 8, 159, 106)
    terminal =
        Terminal.new(editFile, editSprite, loadCart, saveCart, importCode)
    synth = Synth.new(0, 8)
    musicTracker = MusicTracker.new(0, 8)
    mapEditor = MapEditor.new(0, 8)

    currentWindow = terminal
    lastWindow = textEditor
    spriteEditor.init()

    tabs = {
        {width = 9, iconfn = draw_brackets_icon, window = textEditor},
        {width = 7, iconfn = draw_alien_icon, window = spriteEditor},
        {width = 7, iconfn = draw_map_icon, window = mapEditor}, -- }
        {width = 7, iconfn = draw_note_icon, window = musicTracker}
    }
    Intro:init(1.3, 30)
end

function _error(error_message)
    local errorType = "runtime error"
    local errorFirstLine
    local firstLineEndIndex = error_message:find("\n")
    local errorText
    local lineNumber = error_message:match("%[string .-%]:(%d+): (.+)")
    local traceback = error_message:match("stack traceback:\n(.+)")
    local localLine, tabIndex, lineText =
        textEditor:convertLineNumber(tonumber(lineNumber))

    if error_message:find("syntax error") then errorType = "syntax error" end

    -- for runtime error get just firt line for parsing
    if firstLineEndIndex then
        errorFirstLine = error_message:sub(1, firstLineEndIndex - 1)
    else
        errorFirstLine = error_message
    end

    -- get error message
    errorText = errorFirstLine:match("^%[.+:%d+:%s*(.+)")

    -- print regular error
    -- space
    terminal:printLn(">", 3)
    -- general error header
    terminal:printLn(
        errorType .. " line " .. localLine .. " (tab " .. tabIndex .. ")", 1)
    -- line with error
    terminal:printLn(lineText, 3)
    -- error text
    terminal:printLn(errorText, 2)

    -- if we have stacktrace print it too
    if traceback then
        terminal:printLn("stack trace:", 1)
        for line in traceback:gmatch("[^\n]+") do
            local lineNumber, functionName = line:match(
                                                 "^%s*%[string .-%]:(%d+): in function '(.+)'")

            if lineNumber and functionName then
                -- Convert the line number to local line number and tab index
                local localLine, tabIndex, _ =
                    textEditor:convertLineNumber(tonumber(lineNumber))
                local text = "in " .. functionName .. " line " .. localLine ..
                                 " (tab " .. tabIndex .. ")"
                trace(text)
                terminal:printLn(text, 1)
            end
        end
    end

    currentWindow = terminal

    textEditor:switchToTab(tonumber(tabIndex + 1))
    textEditor:setCursor(0, tonumber(localLine) - 1)
end

function _update()
    if Intro.active then return end

    currentWindow:update()
end

function _draw()
    if Intro.active then
        Intro:draw()
        return
    end

    if currentWindow.draw then currentWindow:draw() end

    if currentWindow ~= terminal then drawPanel() end

    if currentWindow.drawPost then currentWindow:drawPost() end

    if currentWindow ~= terminal then drawCursor() end
end

function _key(key_code, ctrl_pressed, shift_pressed)
    local hot_key_pressed = false

    if ctrl_pressed and key_code == KEYCODE.KEY_R then
        trace("run cart")
        saveCart()
        runCart()
        hot_key_pressed = true
    elseif ctrl_pressed and key_code == KEYCODE.KEY_S then
        saveCart()
        hot_key_pressed = true
    elseif key_code == KEYCODE.KEY_F1 then
        hot_key_pressed = true
        currentWindow = textEditor
    elseif key_code == KEYCODE.KEY_F2 then
        hot_key_pressed = true
        currentWindow = spriteEditor
    elseif key_code == KEYCODE.KEY_F3 then
        hot_key_pressed = true
        currentWindow = mapEditor
    elseif key_code == KEYCODE.KEY_F4 then
        hot_key_pressed = true
        currentWindow = musicTracker
    elseif key_code == KEYCODE.KEY_ESCAPE then
        hot_key_pressed = true
        if (currentWindow == terminal) then
            currentWindow = lastWindow
        else
            lastWindow = currentWindow
            currentWindow = terminal
        end
    end

    if not hot_key_pressed then
        currentWindow:key(key_code, ctrl_pressed, shift_pressed)
    end
end

function _keyup(key_code, ctrl_pressed, shift_pressed)
    currentWindow:keyup(key_code, ctrl_pressed, shift_pressed)
end

function _mousep(x, y, button)
    if (y < 7 and x < 37) then
        local offset = 0
        for i = 1, #tabs do
            local tab = tabs[i]
            if (x >= offset and x < offset + tab.width) then
                currentWindow = tab.window
                break
            end
            offset = offset + tab.width
        end
    elseif currentWindow.mousep then
        currentWindow:mousep(x, y, button)
    end
end

function _mouser(x, y, button)
    if currentWindow.mouser then currentWindow:mouser(x, y, button) end
end

function _mousem(x, y, button)
    if currentWindow.mousem then currentWindow:mousem(x, y) end
    mouse_coords = {x = x, y = y}
end

function draw_note_icon(x, y, col)
    rectfill(x, y + 3, 2, 2, col)
    line(x + 1, y, x + 1, y + 3, col)
    line(x + 1, y, x + 4, y, col)
    line(x + 4, y, x + 4, y + 3, col)
    rectfill(x + 3, y + 3, 2, 2, col)
end

function draw_brackets_icon(x, y, col) print("{}", x, y, col) end

function draw_alien_icon(x, y, col)
    line(x, y + 1, x, y + 4, col)
    line(x + 1, y, x + 3, y, col)
    line(x + 4, y + 1, x + 4, y + 4, col)
    line(x + 2, y + 1, x + 2, y + 4, col)
    rectfill(x, y + 2, 5, 2, col)
end

function draw_map_icon(x, y, col)
    rectfill(x, y, 2, 2, col)
    rectfill(x + 3, y, 2, 2, col)
    rectfill(x, y + 3, 2, 2, col)
    rectfill(x + 3, y + 3, 2, 2, col)
end

function drawPanel()
    local offset = 0
    rectfill(0, 0, 160, 7, 2)
    for i = 1, #tabs do
        local tab = tabs[i]
        local index = i - 1

        rectfill(offset, y, tab.width, 7, tab.window == currentWindow and 1 or 2)
        tab.iconfn(offset + 1, 1, tab.window == currentWindow and 3 or 0)
        offset = offset + tab.width
    end
end

function drawCursor()
    local cursor_spr = {
        {0, 0, 4, 4, 4, 4}, {0, 3, 0, 4, 4, 4}, {0, 3, 3, 0, 4, 4},
        {0, 3, 3, 3, 0, 4}, {0, 3, 3, 3, 3, 0}, {0, 3, 0, 0, 0, 4},
        {0, 0, 4, 4, 4, 4}
    }
    for i = 1, #cursor_spr do
        for j = 1, #cursor_spr[i] do
            if cursor_spr[i][j] < 4 then
                pset(mouse_coords.x + j - 1, mouse_coords.y + i - 2,
                     cursor_spr[i][j])
            end
        end
    end
end

function editFile(file)
    local text = read_file(file)
    trace("edit file: " .. file)
    lastWindow = terminal
    currentWindow = textEditor
    textEditor:init(text)
end

function editSprite(file)
    lastWindow = terminal
    currentWindow = spriteEditor
end

function loadCart(path)
    local result, errorMsg, adjustedPath = load_cart(path)

    if errorMsg then
        trace(errorMsg)
        return errorMsg
    else
        trace("cart loaded: " .. adjustedPath)
        textEditor:init(result)
        spriteEditor.init()
        currentCartPath = adjustedPath or path -- Use adjusted path if available
    end
end

function loadAndPlayCart(path)
    local result, errorMsg, adjustedPath = load_cart(path)

    if errorMsg ~= nil then
        trace(errorMsg)
        return errorMsg
    else
        textEditor:init(result)
        currentCartPath = adjustedPath or path -- Use adjusted path if available
        runCart()
    end
end

function saveCart(path)
    if path or currentCartPath then
        trace("writing cart: " .. (path or currentCartPath))
        save_cart((path or currentCartPath), textEditor:getText())

        if path ~= nil then currentCartPath = path end
    else
        trace("cart path is nil")
    end
end

function runCart(path)
    trace("running cart")
    local errorMsg = nil

    if path then errorMsg = loadCart(path) end

    if errorMsg == nil then run_code(textEditor:getText()) end
end

function importCode(code) textEditor:init(code) end
