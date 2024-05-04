local Command = {}
Command.__index = Command

function Command.new()
    local self = setmetatable({}, Command)
    return self
end

function Command:execute()
    -- override in subclass
end

function Command:undo()
    -- override in subclass
end