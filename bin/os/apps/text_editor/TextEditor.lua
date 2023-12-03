local TextEditor = {}
TextEditor.__index = TextEditor
local SyntaxHighlighter = require("os/apps/text_editor/SyntaxHighlighter")
local CursorManager = require("os/apps/text_editor/CursorManager")
local SelectionManager = require("os/apps/text_editor/SelectionManager")
local TextManipulation = require("os/apps/text_editor/TextManipulation")
local InputHandler = require("os/apps/text_editor/InputHandler")
local DisplayManager = require("os/apps/text_editor/DisplayManager")

function TextEditor.new(text, x, y, width, height)
    local self = setmetatable({}, TextEditor)
    self.x = x or 0
    self.y = y or 0
    self.width = width or 0
    self.height = height or 0
    self.offsetY = 0
    self:init(text)

    return self
end

function TextEditor:init(text)
    self.text = text or ""
    self.cursor = {x = 0, y = 0, visible = true}
    self.lines = split(text or "", "\n")
    self.rows_on_screen = ceil(self.height / 6)
    self.cols_on_screen = ceil(self.width / 4)
    self.scroll = {x = 0, y = 0}
    self.selection = {x1 = nil, y1 = nil, x2 = nil, y2 = nil}
    self.colors = {}
    self.syntax_highlighting_dirty = true
    self.selecting = false
    self.cursorBlink = 0
    self.offsetY = 0
end

-- Input

function TextEditor:key(key_code, ctrl_pressed, shift_pressed)
    InputHandler.handleKeyInput(self, key_code, ctrl_pressed, shift_pressed)
end

function TextEditor:mousep(x, y, button)
    InputHandler:handleMouseInput(self, x, y, button)
end

function TextEditor:update()
    if self.syntax_highlighting_dirty == true then
        self:highlightSyntax()
        self.syntax_highlighting_dirty = false
    end

    CursorManager:updateCursor(self)
end

function TextEditor:setPosition(x, y)
    self.x = x
    self.y = y
end

function TextEditor:getPosition() return self.x, self.y end

function TextEditor:setSize(width, height)
    self.width = width
    self.height = height
end

function TextEditor:getSize() return self.width, self.height end

-- Display

function TextEditor:draw() DisplayManager:drawEditor(self) end

function TextEditor:redrawText() DisplayManager:redrawText(self) end

function TextEditor:drawCursor() CursorManager:drawCursor(self) end

-- Text Manipulation

function TextEditor:newLine(x, y) TextManipulation:newLine(self, x, y) end

function TextEditor:removeChar(dir, x, y)
    TextManipulation:removeChar(self, dir, x, y)
end

function TextEditor:removeSelectedText()
    TextManipulation:removeSelectedText(self)
end

function TextEditor:copy() TextManipulation:copy(self) end

function TextEditor:paste() TextManipulation:paste(self) end

function TextEditor:cut() TextManipulation:cut(self) end

function TextEditor:appendText(str, x, y)
    TextManipulation:appendText(self, str, x, y)
end

function TextEditor:setText(str) TextManipulation:setText(self, str) end

function TextEditor:getText() return TextManipulation:getText(self) end

-- Cursor

function TextEditor:homeCursor() CursorManager:homeCursor(self) end

function TextEditor:endCursor() CursorManager:endCursor(self) end

function TextEditor:moveCursorToLineStart()
    CursorManager:moveCursorToLineStart(self)
end

function TextEditor:pageUp() CursorManager:pageUp(self) end

function TextEditor:pageDown() CursorManager:pageDown(self) end

function TextEditor:adjustCursorPositionAndScroll(x, y)
    CursorManager:adjustCursorPositionAndScroll(self, x, y)
end

function TextEditor:moveCursor(xDir, yDir)
    CursorManager:moveCursor(self, xDir, yDir)
end

function TextEditor:isUnderCursor(x, y)
    return CursorManager:isUnderCursor(self, x, y)
end

function TextEditor:setCursor(x, y) CursorManager:setCursor(self, x, y) end

-- Selection

function TextEditor:isSelected(x, y)
    return SelectionManager:isSelected(self, x, y)
end

function TextEditor:selectAll() SelectionManager:selectAll(self) end

function TextEditor:clearSelection() SelectionManager:clearSelection(self) end

function TextEditor:selectText() SelectionManager:selectText(self) end

function TextEditor:startSelection() SelectionManager:startSelection(self) end

function TextEditor:checkSelectionStart()
    SelectionManager:checkSelectionStart(self)
end

function TextEditor:checkSelectionUpdate()
    SelectionManager:checkSelectionUpdate(self)
end

-- Syntax Highlighting

function TextEditor:getColor(x, y) return SyntaxHighlighter:getColor(x, y) end

function TextEditor:highlightSyntax()
    SyntaxHighlighter:clearColors()
    for i = self.scroll.y + 1, self.scroll.y + self.rows_on_screen + 1 do
        local line = self.lines[i]
        if line == nil then break end
        SyntaxHighlighter:highlightLine(line, i)
    end
end

return TextEditor
