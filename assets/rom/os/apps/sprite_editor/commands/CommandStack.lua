local CommandStack = {}
CommandStack.__index = CommandStack

function CommandStack:new()
    local self = setmetatable({}, CommandStack)
    self.stack = {}
    self.index = 0
    return self
end

function CommandStack:push(command)
    -- remove any commands after the current index
    while #self.stack > self.index do
        table.remove(self.stack)
    end
    -- add the new command to the stack
    table.insert(self.stack, command)
    self.index = self.index + 1
    trace("self.index: " .. self.index)
    -- execute the command
    command:execute()
end

function CommandStack:undo()
    trace("CommandStack:undo()")
    trace("self.index: " .. self.index)
    if self.index > 0 then
        -- undo the last command
        local command = self.stack[self.index]
        trace("Command:undo()")
        command:undo()
        self.index = self.index - 1
    end
end

return CommandStack