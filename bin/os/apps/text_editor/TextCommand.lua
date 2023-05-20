local Command = require("Command")

local TextCommand = {}
TextCommand.__index = TextCommand
setmetatable(TextCommand, {
    __index = Command
})

function TextCommand.new(editor, text, line, position)
    local self = setmetatable(Command.new(), TextCommand)
    self.editor = editor
    self.text = text
    self.line = line
    self.position = position
    self.old_text = editor.lines[line]:sub(position + 1, position + #text)
    self.scroll = {x = editor.scroll.x, y = editor.scroll.y}
    return self
end

function TextCommand:execute()
    local line_text = self.editor.lines[self.line]
    self.editor.lines[self.line] = line_text:sub(1, self.position) .. self.text .. line_text:sub(self.position + 1)
    self.editor.cursor.x = self.position + #self.text
    self.editor.syntax_highlighting_dirty = true
end

function TextCommand:undo()
    local line_text = self.editor.lines[self.line]
    self.editor.lines[self.line] = line_text:sub(1, self.position - #self.text) .. self.old_text ..
                                       line_text:sub(self.position + 1)
    self.editor.cursor.x = self.position - #self.text
    self.editor.syntax_highlighting_dirty = true
end

function TextCommand:redo()
    local line_text = self.editor.lines[self.line]
    self.editor.lines[self.line] = line_text:sub(1, self.position) .. self.text ..
                                       line_text:sub(self.position + #self.old_text + 1)
    self.editor.cursor.x = self.position + #self.text
    self.editor.syntax_highlighting_dirty = true
    self.editor.scroll.x = self.scroll.x
    self.editor.scroll.y = self.scroll.y
end

return TextCommand
