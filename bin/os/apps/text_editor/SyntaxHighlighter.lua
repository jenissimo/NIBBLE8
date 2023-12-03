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
    "function", "class", "super", "for", "do", "local", "while", "if", "then",
    "else", "elseif", "return", "continue", "break", "true", "false", "print",
    "begin", "end", "cls", "trace", "pset", "pget", "sget", "sset", "fget",
    "palt", "spr", "sspr", "fset", "mget", "mset", "peek", "poke", "peek4",
    "poke4", "peek2", "poke2", "circ", "circfill", "rect", "rectfill", "line",
    "flr", "sin", "cos", "rnd", "t", "time"
}
local TERMINATORS = {
    "\n", "{", "}", "(", ")", " ", "[", "]", ",", "=", ".", "*", "-", "+", "/",
    " "
}

function SyntaxHighlighter:clearColors()
    self.colors = {}
end

function SyntaxHighlighter:setColor(lineIndex, index, color)
    if self.colors[lineIndex] == nil then self.colors[lineIndex] = {} end
    self.colors[lineIndex][index] = color
end

function SyntaxHighlighter:getColor(lineIndex, index)
    if self.colors[lineIndex] == nil then return SYNTAX_SCHEME.DEFAULT end
    return self.colors[lineIndex][index]
end

function SyntaxHighlighter:peek(line, index)
    if index > #line then return nil end
    return sub(line, index, index)
end

function SyntaxHighlighter:match(line, index, values, terminateValues)
    local found = true
    local resultIndex = nil

    for valueIndex = 1, #values do
        found = true
        for charIndex = 1, #values[valueIndex] do
            if self:peek(line, index + charIndex - 1) ~=
                self:peek(values[valueIndex], charIndex) then
                found = false
                break
            end
        end

        if found == true then
            local endIndex = index + #values[valueIndex]
            local nextChar = self:peek(line, endIndex)
            local prevChar = index > 1 and self:peek(line, index - 1) or nil

            -- Check for word boundaries
            if not (self:isWordCharacter(prevChar) or
                self:isWordCharacter(nextChar)) then
                return endIndex -- Include the last character in the keyword
            end
        end
    end

    return nil
end

function SyntaxHighlighter:isWordCharacter(char) return char and char:match("[%w_]") end

function SyntaxHighlighter:isNumber(char, prevChar)
    -- Check if the character is a digit, but not part of a variable name
    -- A variable name part would be a digit following a letter or an underscore
    if char and char:match("%d") then
        if prevChar and (prevChar:match("[%a_]")) then
            return false -- Part of a variable name, not a standalone number
        else
            return true -- Standalone number or part of a number
        end
    end
    return false
end

function SyntaxHighlighter:highlightString(line, lineIndex, startIndex, quoteType)
    local i = startIndex
    repeat
        SyntaxHighlighter:setColor(lineIndex, i, SYNTAX_SCHEME.STRING)
        i = i + 1
        if i > #line then break end
    until sub(line, i, i) == quoteType
    SyntaxHighlighter:setColor(lineIndex, i, SYNTAX_SCHEME.STRING)
    return i + 1
end

function SyntaxHighlighter:highlightComment(line, lineIndex, startIndex)
    for i = startIndex, #line do
        SyntaxHighlighter:setColor(lineIndex, i, SYNTAX_SCHEME.COMMENT)
    end
    return #line + 1  -- Move to the end of the line
end

function SyntaxHighlighter:highlightKeyword(line, lineIndex, startIndex)
    local endIndex = self:match(line, startIndex, KEYWORDS, TERMINATORS)
    if endIndex then
        for i = startIndex, endIndex - 1 do
            SyntaxHighlighter:setColor(lineIndex, i, SYNTAX_SCHEME.KEYWORD)
        end
        return endIndex
    else
        -- If no keyword is found, move to the next character
        return startIndex + 1
    end
end

function SyntaxHighlighter:highlightNumber(line, lineIndex, startIndex)
    local i = startIndex
    while i <= #line and self:isNumber(sub(line, i, i), sub(line, i - 1, i - 1)) do
        SyntaxHighlighter:setColor(lineIndex, i, SYNTAX_SCHEME.NUMBER)
        i = i + 1
    end
    return i
end

function SyntaxHighlighter:highlightLine(line, lineIndex)
    local i = 1

    self.colors[lineIndex] = {}

    while i <= #line do
        local char = sub(line, i, i)
        local prevChar = i > 1 and sub(line, i - 1, i - 1) or nil

        -- String highlighting (double quotes)
        if char == "\"" then
            i = self:highlightString(line, lineIndex, i, "\"")

            -- String highlighting (single quotes)
        elseif char == "'" then
            i = self:highlightString(line, lineIndex, i, "'")

            -- Comment highlighting
        elseif char == "-" and self:peek(line, i + 1) == "-" then
            self:highlightComment(line, lineIndex, i)
            break

            -- Keyword highlighting
        elseif self:match(line, i, KEYWORDS, TERMINATORS) then
            i = self:highlightKeyword(line, lineIndex, i)

            -- Number highlighting
        elseif self:isNumber(char, prevChar) then
            i = self:highlightNumber(line, lineIndex, i)

            -- Default highlighting
        else
            self.colors[lineIndex][i] = SYNTAX_SCHEME.DEFAULT
            i = i + 1
        end
    end
end

return SyntaxHighlighter