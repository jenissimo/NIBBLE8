local TextEditor = {}
TextEditor.__index = TextEditor

local SYNTAX_SCHEME = {
    ["KEYWORD"] = 2,
    ["FUNCTION"] = 2,
    ["NUMBER"] = 2,
    ["STRING"] = 1,
    ["COMMENT"] = 1,
    ["DEFAULT"] = 3
}
local KEYWORDS = {"function", "class", "super", "for", "do", "local", "while", "if", "then", "else", "elseif", "return",
                  "continue", "break", "true", "false", "print", "begin", "end", "cls", "trace", "pset", "pget", "sget",
                  "sset", "fget", "palt", "spr", "sspr", "fset", "mget", "mset", "peek", "poke", "peek4", "poke4",
                  "peek2", "poke2", "circ", "circfill", "rect", "rectfill", "line"}
local TERMINATORS = {"\n", "{", "}", "(", ")", " ", "[", "]", ",", "=", ".", "*", "-", "+", "/", " "}

function TextEditor.new(text, x, y, width, height)
    local self = setmetatable({}, TextEditor)
    self.x = x or 0
    self.y = y or 0
    self.width = width or 0
    self.height = height or 0
    self:init(text)

    return self
end

function TextEditor:init(text)
    self.text = text or ""
    self.cursor = {
        x = 0,
        y = 0,
        visible = true
    }
    self.lines = split(text or "", "\n")
    self.rows_on_screen = ceil(self.height / 6)
    self.cols_on_screen = ceil(self.width / 4)
    self.scroll = {
        x = 0,
        y = 0
    }
    self.selection = {
        x1 = nil,
        y1 = nil,
        x2 = nil,
        y2 = nil
    }
    self.colors = {}
    self.syntax_highlighting_dirty = true
    self.selecting = false
    self.cursorBlink = 0
end

function TextEditor:key(key_code, ctrl_pressed, shift_pressed)
    self.selecting = false

    -- Check for Hot Keys
    if ctrl_pressed then
        if key_code == KEYCODE.KEY_A then
            trace("CTRL+A")
            self:selectAll()
            return
        elseif key_code == KEYCODE.KEY_C then
            trace("CTRL+C")
            self:copy()
            return
        elseif key_code == KEYCODE.KEY_V then
            trace("CTRL+V")
            self:paste()
            return
        elseif key_code == KEYCODE.KEY_UP then
            trace("CTRL+UP")
            self.selecting = shift_pressed
            self:pageUp()
            return
        elseif key_code == KEYCODE.KEY_DOWN then
            trace("CTRL+DOWN")
            self.selecting = shift_pressed
            self:pageDown()
            return
        elseif key_code == KEYCODE.KEY_LEFT then
            trace("CTRL+LEFT")
            self.selecting = shift_pressed
            self:moveCursorToLineStart()
            return
        elseif key_code == KEYCODE.KEY_RIGHT then
            trace("CTRL+RIGHT")
            self.selecting = shift_pressed
            self:endCursor()
            return
        end
    end

    -- Check for Text Input
    local inputText = UTILS.handle_text_input(key_code, ctrl_pressed, shift_pressed)
    if #inputText > 0 then
        self:appendText(inputText)
        return
    end

    -- Check for Cursor Movement
    if (key_code == KEYCODE.KEY_LEFT) then
        self.selecting = shift_pressed
        self:moveCursor(-1, 0)
        return
    elseif (key_code == KEYCODE.KEY_RIGHT) then
        self.selecting = shift_pressed
        self:moveCursor(1, 0)
        return
    elseif (key_code == KEYCODE.KEY_UP) then
        self.selecting = shift_pressed
        self:moveCursor(0, -1)
        return
    elseif (key_code == KEYCODE.KEY_DOWN) then
        self.selecting = shift_pressed
        self:moveCursor(0, 1)
        return
    elseif (key_code == KEYCODE.KEY_PGUP) then
        self.selecting = shift_pressed
        self:moveCursor(0, -rows_on_screen)
        return
    elseif (key_code == KEYCODE.KEY_PGDOWN) then
        self.selecting = shift_pressed
        self:moveCursor(0, rows_on_screen)
        return
    elseif (key_code == KEYCODE.KEY_HOME) then
        self.selecting = shift_pressed
        -- self:clearSelection()
        self:homeCursor()
        return
    elseif (key_code == KEYCODE.KEY_END) then
        self.selecting = shift_pressed
        -- self:clearSelection()
        self:endCursor()
        return
    end

    -- Check for return
    if (key_code == KEYCODE.KEY_RETURN) then
        self:newLine(self.cursor.x, self.cursor.y)
        self:moveCursor(0, 1);
        self:homeCursor();
        self.syntax_highlighting_dirty = true
        return
    end

    -- Check for backspace
    if (key_code == KEYCODE.KEY_BACKSPACE) then
        trace("Backspace!")
        if self.selection.x1 == nil then
            self:removeChar(-1, self.cursor.x, self.cursor.y)
        else
            self:removeSelectedText()
        end
        return
    end

    -- Check for delete
    if (key_code == KEYCODE.KEY_DELETE) then
        trace("Delete!")
        if selection.x1 == nil then
            self:removeChar(1, self.cursor.x, self.cursor.y)
        else
            self:removeSelectedText()
        end
        return
    end

    -- Check for tab
    if (key_code == KEYCODE.KEY_TAB) then
        self:appendText(" ", 0, cursor.y)
        return
    end
end

function TextEditor:update()
    if self.syntax_highlighting_dirty == true then
        self:highlightSyntax()
        self.syntax_highlighting_dirty = false
    end

    self.cursorBlink = self.cursorBlink + 1

    if self.selection.x1 == nil then
        if self.cursorBlink % 20 == 0 then
            self.cursor.visible = not self.cursor.visible
        end
    else
        self.cursor.visible = false
    end

end

function TextEditor:draw()
    rectfill(self.x - 1, self.y - 1, self.width + 1, self.height + 1, 0)
    if self.cursor.visible == true then
        self:drawCursor()
    end
    self:redrawText()
    self:drawStatusBar()
end

function TextEditor:setPosition(x, y)
    self.x = x
    self.y = y
end

function TextEditor:getPosition()
    return self.x, self.y
end

function TextEditor:setSize(width, height)
    self.width = width
    self.height = height
end

function TextEditor:getSize()
    return self.width, self.height
end

function TextEditor:isSelected(x, y)
    if self.selection.x1 == nil then
        return false
    end

    local x1 = self.selection.x1
    local x2 = self.selection.x2
    local y1 = self.selection.y1
    local y2 = self.selection.y2

    if self.selection.x1 > self.selection.x2 then
        x1 = self.selection.x2
        x2 = self.selection.x1 - 1
    else
        x2 = self.selection.x2 - 1
    end

    if self.selection.y1 > self.selection.y2 then
        y1 = self.selection.y2
        y2 = self.selection.y1
    end

    if y == y1 and y == y2 then
        if x >= x1 and x <= x2 then
            return true
        end
    elseif y == y1 then
        if x >= x1 then
            return true
        end
    elseif y == y2 then
        if x <= x2 then
            return true
        end
    elseif y > y1 and y < y2 then
        return true
    end

    return false
end

function TextEditor:redrawText()
    local colors = self.colors
    local scroll = self.scroll
    local cursor = self.cursor
    local lines = self.lines
    local rows_on_screen = self.rows_on_screen
    local cols_on_screen = self.cols_on_screen

    local index = 0
    for i = self.scroll.y + 1, self.scroll.y + 1 + self.rows_on_screen, 1 do
        if i > #self.lines then
            break
        end
        for j = self.scroll.x + 1, self.scroll.x + self.cols_on_screen + 1, 1 do
            if j > #self.lines[i] then
                break
            end

            local color

            if self.colors[i] == nil or self.colors[i][j] == nil then
                color = SYNTAX_SCHEME.DEFAULT
            else
                color = self.colors[i][j]
            end

            if self.cursor.x + self.scroll.x == j - 1 and self.cursor.y + self.scroll.y == i - 1 and self.cursor.visible then
                color = 3
            end

            if self:isSelected(j - 1, i - 1) == true then
                -- trace("Selected: " .. str(j - 1) .. ", " .. str(i - 1))
                print(sub(self.lines[i], j, j), self.x + (j - self.scroll.x - 1) * 4, index * 6 + self.y, 3, 1)
            else
                print(sub(self.lines[i], j, j), self.x + (j - self.scroll.x - 1) * 4, index * 6 + self.y, color, 0)
            end

        end
        index = index + 1
    end
end

function TextEditor:drawCursor()
    rectfill(self.cursor.x * 4 + self.x, self.cursor.y * 6 + self.y, 4, 5, 2)
end

function TextEditor:newLine(x, y)
    local lineIndex = self.scroll.y + y + 1
    local line = self.lines[lineIndex]
    local newLine = ""

    if x == 0 then
        table.insert(self.lines, lineIndex, newLine)
    else
        newLine = sub(line, 1, self.scroll.x + x)
        self.lines[lineIndex] = sub(line, self.scroll.x + x + 1)
        table.insert(self.lines, lineIndex, newLine)
    end
end

function TextEditor:selectAll()
    self.selection.x1 = 0
    self.selection.y1 = 0
    self.selection.x2 = #self.lines[#self.lines]
    self.selection.y2 = #self.lines - 1

    trace(self.selection.x1 .. ", " .. self.selection.y1 .. " - " .. self.selection.x2 .. ", " .. self.selection.y2)
end

function TextEditor:clearSelection()
    self.selection.x1 = nil
    self.selection.y1 = nil
    self.selection.x2 = nil
    self.selection.y2 = nil
end

function TextEditor:selectText()
    local dirX = 0
    local dirY = 0

    if key(KEYCODE.KEY_LEFT) == 1 then
        dirX = -1
    elseif key(KEYCODE.KEY_RIGHT) == 1 then
        dirX = 1
    elseif key(KEYCODE.KEY_UP) == 1 then
        dirY = -1
    elseif key(KEYCODE.KEY_DOWN) == 1 then
        dirY = 1
    end

    if dirX ~= 0 or dirY ~= 0 then
        if self.selection.x1 == nil then
            self:startSelection()
        else
            self.selection.x2 = self.cursor.x + self.scroll.x
            self.selection.y2 = self.cursor.y + self.scroll.y
        end
    end

    if self.selection.x2 < 0 or self.selection.y2 < 0 then
        self:clearSelection()
    end
end

function TextEditor:startSelection()
    self.selection.x1 = self.cursor.x + self.scroll.x
    self.selection.y1 = self.cursor.y + self.scroll.y
    self.selection.x2 = self.cursor.x + self.scroll.x
    self.selection.y2 = self.cursor.y + self.scroll.y
end

-- TODO: Rewrite check for shift
function TextEditor:checkSelectionStart()
    if self.selecting then
        if self.selection.x1 == nil then
            self:startSelection()
        end
    else
        self:clearSelection()
    end
end

function TextEditor:checkSelectionUpdate()
    if self.selecting then
        self:selectText()
    end
end

function TextEditor:homeCursor()
    self:checkSelectionStart()

    self.scroll.x = 0
    self:setCursor(0, self.cursor.y)

    self:checkSelectionUpdate()
end

function TextEditor:endCursor()
    local line = self.lines[self.scroll.y + self.cursor.y + 1]
    local line_len = #line

    self:checkSelectionStart()

    if line == nil then
        return
    end

    if (line_len > self.cols_on_screen) or ((self.cursor.x + self.scroll.x) > self.cols_on_screen) then
        self.scroll.x = flr(line_len / self.cols_on_screen) * self.cols_on_screen
        self:setCursor(line_len % self.cols_on_screen, self.cursor.y)
    else
        self:setCursor(line_len, self.cursor.y)
    end

    self:checkSelectionUpdate()
end

function TextEditor:moveCursorToLineStart()
    local line = self.lines[self.cursor.y + self.scroll.y]

    if not line then
        return
    end

    local foundNonWhitespace = false

    self:checkSelectionStart()

    for i = 1, #line do
        local currentChar = string.sub(line, i, i)
        if currentChar == " " or currentChar ~= "\t" then
            if (i < self.cols_on_screen) then
                self:setCursor(i, self.cursor.y)
                self.scroll.x = 0
            else
                self:setCursor(self.cols_on_screen, self.cursor.y)
                self.scroll.x = i - self.cols_on_screen
            end
            foundNonWhitespace = true
            break
        end
    end

    if not foundNonWhitespace then
        self:setCursor(0, self.cursor.y)
        self.scroll.x = 0
    end

    self:checkSelectionUpdate()
end

function TextEditor:pageUp()
    self:checkSelectionStart()

    self.scroll.y = self.scroll.y - self.rows_on_screen
    self:setCursor(self.cursor.x, 0)

    if self.scroll.y < 0 then
        self.scroll.y = 0
        self:setCursor(self.cursor.x, 0)
    end
    self.syntax_highlighting_dirty = true

    self:checkSelectionUpdate()
end

function TextEditor:pageDown()
    self:checkSelectionStart()

    self.scroll.y = self.scroll.y + self.rows_on_screen
    self:setCursor(self.cursor.x, self.rows_on_screen - 1)

    if self.scroll.y > #self.lines - self.rows_on_screen then
        self.scroll.y = #self.lines - self.rows_on_screen - 1
        self:setCursor(self.cursor.x, self.rows_on_screen - 1)
    end
    self.syntax_highlighting_dirty = true

    self:checkSelectionUpdate()
end

function TextEditor:adjustCursorPositionAndScroll(x, y)
    -- adjust cursor position if it's outside the visible screen area
    if x < self.scroll.x then
        x = self.scroll.x
    elseif x > self.scroll.x + self.cols_on_screen - 1 then
        x = self.scroll.x + self.cols_on_screen - 1
    end

    if y < self.scroll.y then
        y = self.scroll.y
    elseif y > self.scroll.y + self.rows_on_screen - 1 then
        y = self.scroll.y + self.rows_on_screen - 1
    end

    -- adjust scroll position if cursor is outside visible screen area
    if x < self.scroll.x then
        self.scroll.x = x
    elseif x > self.scroll.x + self.cols_on_screen - 1 then
        self.scroll.x = x - self.cols_on_screen + 1
    end
    if y < self.scroll.y then
        self.scroll.y = y
    elseif y > self.scroll.y + self.rows_on_screen - 1 then
        self.scroll.y = y - self.rows_on_screen + 1
    end
    
    self.cursor.x = x
    self.cursor.y = y
end

function TextEditor:moveCursor(xDir, yDir)
    local newX = self.cursor.x + xDir
    local newY = self.cursor.y + yDir
    local line = self.lines[newY + self.scroll.y + 1]

    self:checkSelectionStart()

    if xDir < 0 and self.cursor.x + self.scroll.x <= 0 and self.cursor.y + self.scroll.y <= 0 then
        return
    end

    if line == nil and yDir < 0 then
        self:homeCursor()
        return
    elseif line == nil and yDir > 0 then
        self:endCursor()
        return
    end

    if (newX + self.scroll.x > #line) and yDir ~= 0 then
        self:setCursor(newX, newY)
        self:endCursor()
        return
    end

    if (newX + self.scroll.x > #line) and (newY + self.scroll.y + 1 < #self.lines) then
        self:setCursor(newX, newY + 1)
        self:homeCursor()
        return
    elseif (newX + self.scroll.x < 0) and (newY + self.scroll.y - 1 >= 0) then
        self:setCursor(newX, newY - 1)
        self:endCursor()
        return
    elseif newX >= self.cols_on_screen then
        self.scroll.x = self.scroll.x + 1
    elseif newX < 0 then
        self.scroll.x = self.scroll.x - 1
    elseif newX >= #line then
        self:setCursor(#line, self.cursor.y)
    else
        self:setCursor(newX, self.cursor.y)
    end

    if newY >= self.rows_on_screen then
        self.scroll.y = self.scroll.y + 1
        self.syntax_highlighting_dirty = true
    elseif newY < 0 then
        self.scroll.y = self.scroll.y - 1
        self.syntax_highlighting_dirty = true
    else
        self:setCursor(self.cursor.x, newY)
    end

    self:checkSelectionUpdate()
end

function TextEditor:removeChar(dir, x, y)
    local lineIndex = self.scroll.y + self.cursor.y + 1
    local line = self.lines[lineIndex]
    local newLine = ""

    if dir == 0 or dir > 0 then
        return
    end

    if dir == -1 then
        if self.scroll.x + self.cursor.x == 0 then
            if self.scroll.y + self.cursor.y == 0 then
                return
            end
            self:moveCursor(0, -1)
            self:endCursor()
            newLine = self.lines[lineIndex - 1] .. line
            self.lines[lineIndex - 1] = newLine
            table.remove(self.lines, lineIndex)
        else
            newLine = sub(line, 1, self.scroll.x + self.cursor.x - 1) .. sub(line, self.scroll.x + self.cursor.x + 1)
            self.lines[lineIndex] = newLine
            self:moveCursor(dir, 0)
        end
    elseif dir == 1 then
        if self.scroll.x + self.cursor.x == #line then
            if self.scroll.y + self.cursor.y == #self.lines then
                return
            end
            newLine = line .. self.lines[lineIndex + 1]
            self.lines[lineIndex] = newLine
            table.remove(self.lines, lineIndex + 1)
        else
            newLine = sub(line, 1, self.scroll.x + self.cursor.x) .. sub(line, self.scroll.x + self.cursor.x + 2)
            self.lines[lineIndex] = newLine
        end
    end

    self.syntax_highlighting_dirty = true
end

function TextEditor:removeSelectedText()
    if self.selection.x1 == nil then
        return
    end

    -- Invert the selection
    local startX, endX, startY, endY
    if self.selection.x1 < self.selection.x2 then
        startX = self.selection.x1
        endX = self.selection.x2
    else
        startX = self.selection.x2
        endX = self.selection.x1
    end
    if self.selection.y1 < self.selection.y2 then
        startY = self.selection.y1
        endY = self.selection.y2
    else
        startY = self.selection.y2
        endY = self.selection.y1
    end

    if startX == endX and startY == endY then
        self:clearSelection()
        return
    end

    trace("Remove selected text")
    trace(str(startX) .. " " .. str(startY) .. " " .. str(endX) .. " " .. str(endY))

    local line
    local line1
    local line2

    if endY - startY >= 1 then
        line = self.lines[endY + 1]
        line1 = sub(line, 1, startX)
        line2 = sub(line, endX + 1)
        self.lines[endY + 1] = line1 .. line2

        for i = endY - 1, startY, -1 do
            trace(str(i))
            table.remove(self.lines, i + 1)
        end
    else
        line = self.lines[startY + 1]
        line1 = sub(line, 1, startX)
        line2 = sub(line, endX)
        self.lines[startY + 1] = line1 .. line2
    end

    self:adjustCursorPositionAndScroll(startX, startY)
    self:clearSelection()
    self.syntax_highlighting_dirty = true
end

function TextEditor:copy()
    if self.selection.x1 == nil then
        return
    end

    -- Invert the selection
    local startX, endX, startY, endY
    if self.selection.x1 < self.selection.x2 then
        startX = self.selection.x1
        endX = self.selection.x2
    else
        startX = self.selection.x2
        endX = self.selection.x1
    end
    if self.selection.y1 < self.selection.y2 then
        startY = self.selection.y1
        endY = self.selection.y2
    else
        startY = self.selection.y2
        endY = self.selection.y1
    end

    if startX == endX and startY == endY then
        self:clearSelection()
        return
    end

    local line
    local line1
    local line2
    local text = ""

    if endY - startY >= 1 then
        line = self.lines[startY + 1]
        line1 = sub(line, startX)
        text = text .. line1 .. "\n"

        for i = startY + 1, endY do
            line = self.lines[i + 1]
            text = text .. line .. "\n"
        end
    else
        line = self.lines[startY + 1]
        line1 = sub(line, 1, startX)
        line2 = sub(line, endX)
        text = text .. line1 .. line2
    end

    trace("Copy text: " .. text)
    set_clipboard_text(text)
end

function TextEditor:paste()
    local text = get_clipboard_text()
    if text == nil then
        return
    end
    local newLines = split(text, "\n")
    trace("Lines count: " .. str(#newLines))

    self:removeSelectedText()

    for i = 1, #newLines do
        trace("Paste text: " .. newLines[i])
        for j = 1, #newLines[i] do
            self:appendText(sub(newLines[i], j, j))
        end
        if i < #newLines then
            self:newLine(self.cursor.x, self.cursor.y)
            self:moveCursor(0, 1);
            self:homeCursor();
        end
    end
end

function TextEditor:appendText(str, x, y)
    if #str == 0 then
        return
    end
    if x == nil then
        x = self.cursor.x
    end
    if y == nil then
        y = self.cursor.y
    end

    local lineIndex = self.scroll.y + y + 1
    local line = self.lines[lineIndex]
    local newLine = ""

    if x == 0 then
        newLine = str .. line
    else
        newLine = sub(line, 1, self.scroll.x + x) .. str .. sub(line, self.scroll.x + x + 1)
    end

    self.lines[lineIndex] = newLine
    self:moveCursor(1, 0)
    self.syntax_highlighting_dirty = true
end

function TextEditor:peek(line, index)
    if index > #line then
        return nil
    end
    return sub(line, index, index)
end

function TextEditor:match(line, index, values, terminateValues)
    local found = true
    local resultIndex = nil

    for valueIndex = 1, #values do
        found = true
        for charIndex = 1, #values[valueIndex] do
            if self:peek(line, index + charIndex - 1) ~= self:peek(values[valueIndex], charIndex) then
                found = false
                break
            end
        end

        if found == true then
            resultIndex = index + #values[valueIndex]

            if resultIndex >= #line then
                return resultIndex
            end
            if terminateValues == nil then
                return resultIndex
            end

            for j = 1, #terminateValues do
                if self:peek(line, resultIndex) == terminateValues[j] then
                    return resultIndex
                end
            end
        end
    end

    return nil
end

function TextEditor:highlightLine(lineIndex)
    local i = 1
    local line = self.lines[lineIndex]

    while i < #line do
        local nextIndex = i
        local char = sub(line, i, i)

        if char == "\"" then
            repeat
                self.colors[lineIndex][nextIndex] = SYNTAX_SCHEME.STRING
                nextIndex = nextIndex + 1
                if nextIndex > #line then
                    break
                end
            until sub(line, nextIndex, nextIndex) == "\""
            self.colors[lineIndex][nextIndex] = SYNTAX_SCHEME.STRING
            nextIndex = nextIndex + 1
        elseif char == "'" then
            nextIndex = i + 1
            while sub(line, nextIndex, nextIndex) ~= "'" do
                self.colors[lineIndex][nextIndex] = SYNTAX_SCHEME.STRING
                nextIndex = nextIndex + 1
                if nextIndex > #line then
                    break
                end
            end
        elseif char == "-" and self:peek(line, i + 1) == "-" then
            for j = i, #line do
                self.colors[lineIndex][j] = SYNTAX_SCHEME.COMMENT
            end
            break
        elseif self:match(line, i, KEYWORDS, TERMINATORS) ~= nil then
            nextIndex = self:match(line, i, KEYWORDS, TERMINATORS)
            for j = i, nextIndex do
                self.colors[lineIndex][j] = SYNTAX_SCHEME.KEYWORD
            end
        else
            self.colors[lineIndex][i] = SYNTAX_SCHEME.DEFAULT
            nextIndex = nextIndex + 1
        end

        i = nextIndex
    end
end

function TextEditor:mousep(x, y, button)
    trace("Mouse pressed")
    if button == 1 then
        self:clearSelection()
        -- glyphs is 6x4
        -- detect cursor position based on glyph size
        -- check if cursor is pointing at character
        -- if so, set cursor position
        local desiredCursorX = math.floor((x - self.x) / 4)
        local desiredCursorY = math.floor((y - self.y) / 6)
        if desiredCursorY + self.scroll.y > #self.lines then
            return
        end

        if desiredCursorX + self.scroll.x > #self.lines[desiredCursorY + self.scroll.y + 1] then
            return
        end

        self:setCursor(desiredCursorX, desiredCursorY)
    end
end

function TextEditor:setCursor(x, y)
    self.cursor.x = x
    self.cursor.y = y
    -- refactor this and make cursor visible
    self:drawCursor()
end

function TextEditor:highlightSyntax()
    self.colors = {}
    for i = self.scroll.y + 1, self.scroll.y + self.rows_on_screen + 1 do
        local line = self.lines[i]
        if line == nil then
            break
        end
        self.colors[i] = {}
        self:highlightLine(i)
    end
end

function TextEditor:setText(str)
    trace("Set text: " + str)
    self.text = str
    self.lines = split(str, "\n")

    self:setCursor(0, 0)
    self.scroll.x = 0
    self.scroll.y = 0
    self:clearSelection()
    self.syntax_highlighting_dirty = true
end

function TextEditor:getText()
    self.text = table.concat(self.lines, "\n")
    return self.text
end

function TextEditor:drawStatusBar()
    local status = "line: " .. str(self.cursor.y + self.scroll.y + 1) .. "/" .. str(#self.lines)

    rectfill(0, 113, 160, 7, 2)
    print(status, 1, 114, 0)
end

return TextEditor
