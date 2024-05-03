local CursorManager = {}

function CursorManager:setCursor(textEditor, x, y)
    if x < 0 then
        x = 0
    elseif x >= textEditor.cols_on_screen - 1 then
        textEditor.cursor.x = textEditor.cols_on_screen - 1
        textEditor.cursor.y = y
        textEditor.scroll.x = textEditor.scroll.x +
                                  (x - (textEditor.cols_on_screen - 1))
    else
        textEditor.cursor.x = x
        textEditor.cursor.y = y
    end

    if y < 0 then
        y = 0
    elseif y == 0 then
        textEditor.offsetY = 0
    elseif y >= textEditor.rows_on_screen - 1 then
        textEditor.offsetY = -3
        textEditor.cursor.y = textEditor.rows_on_screen - 1
        textEditor.scroll.y = textEditor.scroll.y +
                                  (y - (textEditor.rows_on_screen - 1))
    end

    self:showCursor(textEditor)
end

function CursorManager:moveCursor(textEditor, xDir, yDir)
    -- Calculate the new cursor position relative to the entire document
    local documentX = textEditor.cursor.x + textEditor.scroll.x + xDir
    local documentY = textEditor.cursor.y + textEditor.scroll.y + yDir
    local initialScrollX = textEditor.scroll.x
    local initialScrollY = textEditor.scroll.y

    textEditor:checkSelectionStart()

    self:showCursor(textEditor)

    -- Check for homeCursor or endCursor conditions
    if documentY < 0 and textEditor.scroll.y == 0 then
        -- At the top of the document, move to start of the line
        self:homeCursor(textEditor)
        return
    elseif documentY >= #textEditor.lines and yDir > 0 then
        -- Attempting to move down at the bottom of the document, move to end of the line
        self:endCursor(textEditor)
        return
    elseif documentX < 0 and xDir < 0 and documentY > 0 then
        self:moveCursor(textEditor, 0, -1)
        self:endCursor(textEditor)
        return
    elseif documentX > #textEditor.lines[documentY + 1] and xDir > 0 then
        if (documentY + 1) < #textEditor.lines then
            self:moveCursor(textEditor, 0, 1)
            self:homeCursor(textEditor)
        end
        return
    end

    -- Update the scroll position to ensure the cursor stays visible
    local maxLineIndex = #textEditor.lines
    local maxLineLength = 0
    if textEditor.lines[documentY + 1] then
        maxLineLength = #textEditor.lines[documentY + 1]
    end

    if xDir < 0 and textEditor.cursor.x == 0 then
        textEditor.scroll.x = math.max(0, textEditor.scroll.x - 1)
    elseif xDir > 0 and textEditor.cursor.x >= textEditor.cols_on_screen - 1 then
        textEditor.scroll.x = documentX - textEditor.cols_on_screen + 1
    end

    -- Adjust scroll.y when cursor moves up beyond the top of the screen
    if yDir < 0 and textEditor.cursor.y == 0 then
        textEditor.scroll.y = math.max(0, textEditor.scroll.y - 1)
    elseif yDir > 0 and textEditor.cursor.y >= textEditor.rows_on_screen - 1 then
        textEditor.scroll.y = math.max(0,
                                       math.min(
                                           documentY - textEditor.rows_on_screen +
                                               1, maxLineIndex -
                                               textEditor.rows_on_screen))
    end

    -- Update the cursor's position within the bounds of the screen
    local newX = math.min(math.max(documentX - textEditor.scroll.x, 0),
                          textEditor.cols_on_screen - 1)
    local newY = math.min(math.max(documentY - textEditor.scroll.y, 0),
                          textEditor.rows_on_screen - 1)
    self:setCursor(textEditor, newX, newY)

    if documentX > #textEditor.lines[documentY + 1] and yDir ~= 0 then
        self:endCursor(textEditor)
    end

    -- Set the flag only if the scroll has changed
    --    if textEditor.scroll.x ~= initialScrollX or textEditor.scroll.y ~=
    --      initialScrollY then textEditor.syntax_highlighting_dirty = true end
    textEditor.syntax_highlighting_dirty = true
    textEditor:checkSelectionUpdate()
end

function CursorManager:checkAndAdjustCursorBounds(textEditor)
    local totalLines = #textEditor.lines
    local cursorX, cursorY = textEditor.cursor.x + textEditor.scroll.x,
                             textEditor.cursor.y + textEditor.scroll.y

    -- Adjust cursor Y position if it's outside the range of total lines
    if cursorY < 1 then
        cursorY = 1
    elseif cursorY > totalLines then
        cursorY = totalLines
    end

    -- Adjust cursor X position based on the length of the current line
    local currentLineLength = textEditor.lines[cursorY] and
                                  #textEditor.lines[cursorY] or 0
    if cursorX < 1 then
        cursorX = 1
    elseif cursorX > currentLineLength + 1 then
        cursorX = currentLineLength + 1
    end

    -- Adjust scroll position to make sure the cursor is within the visible area
    if cursorX < textEditor.scroll.x + 1 then
        textEditor.scroll.x = cursorX - 1
    elseif cursorX > textEditor.scroll.x + textEditor.cols_on_screen then
        textEditor.scroll.x = cursorX - textEditor.cols_on_screen
    end

    if cursorY < textEditor.scroll.y + 1 then
        textEditor.scroll.y = cursorY - 1
    elseif cursorY > textEditor.scroll.y + textEditor.rows_on_screen then
        textEditor.scroll.y = cursorY - textEditor.rows_on_screen
    end

    -- Update cursor position within the bounds of the screen
    self:setCursor(textEditor, cursorX - textEditor.scroll.x,
                   cursorY - textEditor.scroll.y)
end

function CursorManager:moveCursorToLineStart(textEditor)
    local line = textEditor.lines[textEditor.cursor.y + textEditor.scroll.y + 1]

    trace(line)

    if not line then return end

    local foundNonWhitespace = false
    textEditor:checkSelectionStart()

    for i = 1, #line do
        local currentChar = string.sub(line, i, i)
        if currentChar ~= " " and currentChar ~= "\t" then
            local newCursorX = (i < textEditor.cols_on_screen) and i or
                                   textEditor.cols_on_screen
            self:setCursor(textEditor, newCursorX - 1, textEditor.cursor.y)
            textEditor.scroll.x = (i < textEditor.cols_on_screen) and 0 or
                                      (i - textEditor.cols_on_screen)
            foundNonWhitespace = true
            break
        end
    end

    if not foundNonWhitespace then
        self:setCursor(textEditor, 0, textEditor.cursor.y)
        textEditor.scroll.x = 0
    end

    textEditor:checkSelectionUpdate()
end

function CursorManager:homeCursor(textEditor)
    textEditor:checkSelectionStart()

    textEditor.scroll.x = 0
    self:setCursor(textEditor, 0, textEditor.cursor.y)

    textEditor:checkSelectionUpdate()
end

function CursorManager:endCursor(textEditor)
    local line = textEditor.lines[textEditor.scroll.y + textEditor.cursor.y + 1]
    local line_len = #line

    textEditor:checkSelectionStart()

    if line == nil then return end

    if (line_len > textEditor.cols_on_screen) or
        ((textEditor.cursor.x + textEditor.scroll.x) > textEditor.cols_on_screen) then
        textEditor.scroll.x = flr(line_len / textEditor.cols_on_screen) *
                                  textEditor.cols_on_screen
        self:setCursor(textEditor, line_len % textEditor.cols_on_screen,
                       textEditor.cursor.y)
    else
        self:setCursor(textEditor, line_len, textEditor.cursor.y)
    end

    textEditor:checkSelectionUpdate()
end

function CursorManager:pageUp(textEditor)
    textEditor:checkSelectionStart()

    textEditor.scroll.y = textEditor.scroll.y - textEditor.rows_on_screen
    self:setCursor(textEditor, textEditor.cursor.x, 0)

    if textEditor.scroll.y < 0 then
        textEditor.scroll.y = 0
        self:setCursor(textEditor, textEditor.cursor.x, 0)
    end
    textEditor.syntax_highlighting_dirty = true

    textEditor:checkSelectionUpdate()
end

function CursorManager:pageDown(textEditor)
    textEditor:checkSelectionStart()

    textEditor.scroll.y = textEditor.scroll.y + textEditor.rows_on_screen
    self:setCursor(textEditor, textEditor.cursor.x,
                   textEditor.rows_on_screen - 1)

    if textEditor.scroll.y > #textEditor.lines - textEditor.rows_on_screen then
        textEditor.scroll.y = #textEditor.lines - textEditor.rows_on_screen - 1
        self:setCursor(textEditor, textEditor.cursor.x,
                       textEditor.rows_on_screen - 1)
    end
    textEditor.syntax_highlighting_dirty = true

    textEditor:checkSelectionUpdate()
end

function CursorManager:updateCursor(textEditor)
    textEditor.cursorBlink = textEditor.cursorBlink + 1

    if textEditor.selection.x1 == nil then
        if textEditor.cursorBlink % 20 == 0 then
            textEditor.cursor.visible = not textEditor.cursor.visible
        end
    else
        textEditor.cursor.visible = false
    end
end

function CursorManager:isUnderCursor(textEditor, charDocumentX, charDocumentY)
    -- Calculate the absolute document position of the cursor
    local cursorDocumentX = textEditor.cursor.x + textEditor.scroll.x
    local cursorDocumentY = textEditor.cursor.y + textEditor.scroll.y

    -- Check if the character position matches the cursor position
    return cursorDocumentX == charDocumentX and cursorDocumentY == charDocumentY
end

function CursorManager:showCursor(textEditor)
    textEditor.cursorBlink = 0
    textEditor.cursor.visible = true
end

function CursorManager:drawCursor(textEditor)
    rectfill(textEditor.cursor.x * 4 + textEditor.x,
             textEditor.cursor.y * 6 + textEditor.y + textEditor.offsetY, 3, 5,
             1)

    local cursorX = textEditor.cursor.x + textEditor.scroll.x
    local cursorY = textEditor.cursor.y + textEditor.scroll.y

    if cursorY >= #textEditor.lines then return end

    local cursorLine = textEditor.lines[cursorY + 1]
    local cursorChar = cursorLine:sub(cursorX + 1, cursorX + 1)

    print(cursorChar, textEditor.x + textEditor.cursor.x * 4,
          textEditor.y + textEditor.cursor.y * 6 + textEditor.offsetY, 3)
end

return CursorManager
