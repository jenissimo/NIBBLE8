local SyntaxHighlighter = {}
local SYNTAX_SCHEME = {
    ["KEYWORD"] = 2,
    ["FUNCTION"] = 2,
    ["NUMBER"] = 1,
    ["STRING"] = 1,
    ["COMMENT"] = 1,
    ["DEFAULT"] = 3
}
local KEYWORDS = {
    ["function"] = true,
    ["class"] = true,
    ["super"] = true,
    ["for"] = true,
    ["do"] = true,
    ["local"] = true,
    ["while"] = true,
    ["if"] = true,
    ["then"] = true,
    ["else"] = true,
    ["elseif"] = true,
    ["return"] = true,
    ["continue"] = true,
    ["break"] = true,
    ["true"] = true,
    ["false"] = true,
    ["print"] = true,
    ["begin"] = true,
    ["end"] = true,
    ["cls"] = true,
    ["trace"] = true,
    ["pset"] = true,
    ["pget"] = true,
    ["sget"] = true,
    ["sset"] = true,
    ["fget"] = true,
    ["cpal"] = true,
    ["palt"] = true,
    ["pal"] = true,
    ["spr"] = true,
    ["sspr"] = true,
    ["fset"] = true,
    ["mget"] = true,
    ["mset"] = true,
    ["peek"] = true,
    ["poke"] = true,
    ["peek2"] = true,
    ["poke2"] = true,
    ["circ"] = true,
    ["circfill"] = true,
    ["rect"] = true,
    ["rectfill"] = true,
    ["line"] = true,
    ["flr"] = true,
    ["sin"] = true,
    ["cos"] = true,
    ["rnd"] = true,
    ["mid"] = true,
    ["min"] = true,
    ["max"] = true,
    ["t"] = true,
    ["time"] = true,
    ["pairs"] = true,
    ["ipairs"] = true,
    ["in"] = true,
    ["and"] = true,
    ["not"] = true,
    ["or"] = true,
    ["camera"] = true,
    ["map"] = true,
    ["btn"] = true,
    ["btnp"] = true,
    ["cursor"] = true,
    ["color"] = true,
    ["add"] = true,
    ["str"] = true,
    ["sfx"] = true,
    ["music"] = true
}
local TERMINATORS = {
    ["\n"] = true,
    ["{"] = true,
    ["}"] = true,
    ["("] = true,
    [")"] = true,
    [" "] = true,
    ["["] = true,
    ["]"] = true,
    [","] = true,
    ["="] = true,
    ["."] = true,
    ["*"] = true,
    ["-"] = true,
    ["+"] = true,
    ["/"] = true
}

function SyntaxHighlighter:clearColors(lineIndex)
    if not self.colors then self.colors = {} end

    if lineIndex == nil then
        self.colors = {}
        return
    end

    self.colors[lineIndex] = {}
end

function SyntaxHighlighter:setColor(lineIndex, index, color)
    if not self.colors[lineIndex] then self.colors[lineIndex] = {} end
    self.colors[lineIndex][index] = color
end

function SyntaxHighlighter:getColor(lineIndex, index)
    return self.colors[lineIndex] and self.colors[lineIndex][index] or
               SYNTAX_SCHEME.DEFAULT
end

function SyntaxHighlighter:highlightComment(line, lineIndex, startIndex)
    for i = startIndex, #line do
        self:setColor(lineIndex, i, SYNTAX_SCHEME.COMMENT)
    end
    return #line + 1
end

function SyntaxHighlighter:highlightString(line, lineIndex, startIndex,
                                           quoteType)
    local i = startIndex
    self:setColor(lineIndex, i, SYNTAX_SCHEME.STRING)
    i = i + 1
    while i <= #line do
        local char = sub(line, i, i)
        if char == quoteType and sub(line, i - 1, i - 1) ~= "\\" then
            self:setColor(lineIndex, i, SYNTAX_SCHEME.STRING)
            return i + 1
        end
        self:setColor(lineIndex, i, SYNTAX_SCHEME.STRING)
        i = i + 1
    end
    return i
end

function SyntaxHighlighter:highlightNumber(line, lineIndex, startIndex)
    local i = startIndex
    local isHex = (sub(line, i, i + 1) == "0x" or sub(line, i, i + 1) == "0X")
    local pattern = isHex and "[%dA-Fa-f]" or "%d"

    if isHex then
        self:setColor(lineIndex, i, SYNTAX_SCHEME.NUMBER) -- Color '0'
        self:setColor(lineIndex, i + 1, SYNTAX_SCHEME.NUMBER) -- Color 'x'
        i = i + 2
    end

    while i <= #line and (sub(line, i, i):match(pattern) or
        (not isHex and sub(line, i, i) == "." and
            sub(line, i + 1, i + 1):match("%d"))) do
        self:setColor(lineIndex, i, SYNTAX_SCHEME.NUMBER)
        i = i + 1
    end
    return i
end

function SyntaxHighlighter:highlightKeyword(line, lineIndex, startIndex)
    -- Find the start of a potential keyword or identifier
    local wordStart, wordEnd, word = line:find("([%w_]+)", startIndex)
    if not wordStart then return startIndex end -- Move past non-alphanumeric characters

    if KEYWORDS[word] then
        -- Ensure the word is isolated by checking the characters around the keyword
        local precedingChar = wordStart > 1 and
                                  sub(line, wordStart - 1, wordStart - 1) or " "
        local followingChar = wordEnd < #line and
                                  sub(line, wordEnd + 1, wordEnd + 1) or " "

        -- Check for non-word characters (non-alphanumeric and not '_')
        if not precedingChar:match("[%w_]") and not followingChar:match("[%w_]") then
            for j = wordStart, wordEnd do
                self:setColor(lineIndex, j, SYNTAX_SCHEME.KEYWORD)
            end
            return wordEnd + 1
        end
    end
    -- If not a keyword, continue from the end of the current word
    return wordEnd + 1
end

function SyntaxHighlighter:highlightLine(line, lineIndex)
    self:clearColors(lineIndex)
    local i = 1
    while i <= #line do
        local char = sub(line, i, i)
        local nextChar = (i < #line) and sub(line, i + 1, i + 1) or ""
        local prevChar = (i > 1) and sub(line, i - 1, i - 1) or ""

        if char == "\"" or char == "'" then
            i = self:highlightString(line, lineIndex, i, char)
        elseif char == "-" and nextChar == "-" then
            self:highlightComment(line, lineIndex, i)
            break
        elseif char:match("%d") or
            (char == "0" and (nextChar == "x" or nextChar == "X")) then
            i = self:highlightNumber(line, lineIndex, i)
        elseif char:match("[%w_]") then
            i = self:highlightKeyword(line, lineIndex, i)
        else
            self:setColor(lineIndex, i, SYNTAX_SCHEME.DEFAULT)
            i = i + 1
        end
    end
end

function SyntaxHighlighter:cacheLine(textEditor, lineIndex)
    local line = textEditor.lines[lineIndex] or ""
    local drawText = ""
    local previousColor, previousBgColor = -1, -1

    for charIndex = 1, #line do
        local char = line:sub(charIndex, charIndex)
        local color = self:getColor(lineIndex, charIndex)
        local bgColor = 0 -- Assume default background color for now

        -- Check if the character is part of a selection.
        if textEditor:isSelected(charIndex - 1, lineIndex - 1) then
            color = 3
            bgColor = 1
        end

        -- Insert color control codes before the character as needed.
        if charIndex == 1 or previousColor ~= color or previousBgColor ~=
            bgColor then
            if color ~= previousColor then
                drawText = drawText .. chr(12) .. str(color)
            end
            if bgColor ~= previousBgColor then
                drawText = drawText .. chr(14) .. str(bgColor)
            end
        end

        drawText = drawText .. char
        previousColor, previousBgColor = color, bgColor
    end

    return drawText
end

function SyntaxHighlighter:cacheVisibleLines(textEditor)
    -- Determine the range of lines to draw, considering partial visibility.
    local firstLineIndex = math.max(1, math.floor(textEditor.scroll.y) + 1)
    local lastLineIndex = firstLineIndex + textEditor.rows_on_screen - 1

    -- Ensure we don't try to draw more lines than exist.
    lastLineIndex = math.min(lastLineIndex, #textEditor.lines)
    textEditor.drawText = ""

    for lineIndex = firstLineIndex, lastLineIndex do
        local drawText = self:cacheLine(textEditor, lineIndex)
        textEditor.drawText = textEditor.drawText .. drawText .. "\n"
    end
end

return SyntaxHighlighter
