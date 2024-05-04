local EventDispatcher = {}

function EventDispatcher:new()
  local newObj = {}
  self.__index = self
  return setmetatable(newObj, self)
end

function EventDispatcher:addEventListener(event, callback)
  self.listeners = self.listeners or {}
  self.listeners[event] = self.listeners[event] or {}
  table.insert(self.listeners[event], callback)
end

function EventDispatcher:dispatchEvent(event, ...)
  if self.listeners and self.listeners[event] then
    for _, callback in ipairs(self.listeners[event]) do
      callback(...)
    end
  end
end

return EventDispatcher