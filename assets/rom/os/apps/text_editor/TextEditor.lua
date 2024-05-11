local TextEditor = {}
TextEditor.__index = TextEditor
local SyntaxHighlighter = require("os/apps/text_editor/SyntaxHighlighter")
local CursorManager = require("os/apps/text_editor/CursorManager")
local SelectionManager = require("os/apps/text_editor/SelectionManager")
local TextManipulation = require("os/apps/text_editor/TextManipulation")
local InputHandler = require("os/apps/text_editor/InputHandler")
local DisplayManager = require("os/apps/text_editor/DisplayManager")
local TabBar = require("os/apps/text_editor/TabBar")

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
    if not text then
        return
    end

    self.text = text or ""
    self.cursor = {x = 0, y = 0, visible = true}
    -- self.lines = split(text or "", "\n")
    self.lines = {}
    self.tabs = {}
    self.currentTab = 1
    self.rows_on_screen = ceil(self.height / 6)
    self.cols_on_screen = ceil(self.width / 4)
    self.scroll = {x = 0, y = 0}
    self.selection = {x1 = nil, y1 = nil, x2 = nil, y2 = nil}
    self.colors = {}
    self.syntax_highlighting_dirty = true
    self.selecting = false
    self.cursorBlink = 0
    self.offsetY = 0
    self:parseTabs()
end

function TextEditor:convertLineNumber(lineIndex)
    local lines = split(self.text, "\n")
    local tabIndex = 0
    local tabLine = 0
    local lineText = ""

    for index, line in ipairs(lines) do
        if index == lineIndex + 1 then
            break
        end

        if line:match("^--#tab") then
            tabIndex = tabIndex + 1
            tabLine = index
        end
    end

    return lineIndex - tabLine, tabIndex, lines[lineIndex]
end

-- Tabs
function TextEditor:parseTabs()
    local lines = split(self.text, "\n")
    local currentTabContent = {}
    local tabIndex = 1

    for _, line in ipairs(lines) do
        if line:match("^--#tab") then
            if #currentTabContent > 0 or tabIndex == 1 then
                self:createTab(currentTabContent)
                tabIndex = tabIndex + 1
                currentTabContent = {}
            end
        else
            table.insert(currentTabContent, line)
        end
    end

    -- Create a tab for any remaining content or if no tabs have been created yet
    if #currentTabContent > 0 or tabIndex == 1 then
        self:createTab(currentTabContent)
    end
    TabBar.dirty = true

    self:switchToTab(1) -- Initialize with the first tab
end

function TextEditor:createTab(contentLines)
    table.insert(self.tabs, {
        text = table.concat(contentLines, "\n"),
        cursor = {x = 0, y = 0, visible = true},
        scroll = {x = 0, y = 0},
        selection = {x1 = nil, y1 = nil, x2 = nil, y2 = nil},
        offsetY = 0
    })
    --trace("Created tab: " .. #self.tabs)
end

function TextEditor:switchToTab(tabIndex, checkEmptyTab)
    if tabIndex ~= self.currentTab then TextManipulation:saveCurrentTab(self) end
    -- Check if the current tab is empty before switching
    if checkEmptyTab and self.currentTab > 1 and #self.tabs > 1 and
        self.tabs[self.currentTab] and
        #self.tabs[self.currentTab].text:gsub("%s+", "") == 0 then
        -- Remove the empty tab
        table.remove(self.tabs, self.currentTab)

        -- Adjust the tabIndex if it's greater than the number of tabs
        if tabIndex > #self.tabs then tabIndex = #self.tabs end

        -- Adjust the currentTab index if it's now out of range
        if self.currentTab > #self.tabs then self.currentTab = #self.tabs end
    end

    -- Proceed with switching tabs
    if self.tabs[tabIndex] then
        self.currentTab = tabIndex
        self.lines = split(self.tabs[tabIndex].text, "\n")
        self.cursor = self.tabs[tabIndex].cursor
        self.selection = self.tabs[tabIndex].selection
        self.offsetY = self.tabs[tabIndex].offsetY
        self.scroll = self.tabs[tabIndex].scroll
        self.syntax_highlighting_dirty = true
    end

    TabBar:updateTabSwitch(self, tabIndex)
end

-- Input
function TextEditor:key(key_code, ctrl_pressed, shift_pressed)
    InputHandler.handleKeyInput(self, key_code, ctrl_pressed, shift_pressed)
end

function TextEditor:mousem(x, y, button) TabBar:mousem(self, x, y, button) end

function TextEditor:mousep(x, y, button)
    InputHandler:handleMouseInput(self, x, y, button)
    TabBar:mousep(self, x, y, button)
end

function TextEditor:update()
    if self.syntax_highlighting_dirty == true then
        self:highlightSyntax()
        self.syntax_highlighting_dirty = false
    end

    CursorManager:updateCursor(self)
    TabBar:update(self)
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

function TextEditor:drawPost() TabBar:draw(self) end

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

function TextEditor:checkAndAdjustCursorBounds()
    CursorManager:checkAndAdjustCursorBounds(self)
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

function TextEditor:cacheLine(lineIndex)
    return SyntaxHighlighter:cacheLine(self, lineIndex)
end

function TextEditor:highlightSyntax()
    SyntaxHighlighter:clearColors()
    for i = self.scroll.y + 1, self.scroll.y + self.rows_on_screen + 1 do
        local line = self.lines[i]
        if line == nil then break end
        SyntaxHighlighter:highlightLine(line, i)
    end
    SyntaxHighlighter:cacheVisibleLines(self)
end

return TextEditor
