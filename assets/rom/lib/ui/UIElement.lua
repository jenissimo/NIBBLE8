local UIElement = {}
UIElement.__index = UIElement

function UIElement.new(x, y, width, height)
    local self = setmetatable({}, UIElement)
    self.x = x or 0
    self.y = y or 0
    self.width = width or 0
    self.height = height or 0
    return self
end

function UIElement:setPosition(x, y)
    self.x = x
    self.y = y
end

function UIElement:getPosition()
    return self.x, self.y
end

function UIElement:setSize(width, height)
    self.width = width
    self.height = height
end

function UIElement:getSize()
    return self.width, self.height
end

function UIElement:draw()
    -- This will be overridden by subclasses
end

function UIElement:update()
    -- This will be overridden by subclasses
end

return UIElement
