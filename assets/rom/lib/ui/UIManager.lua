local UIManager = {}
UIManager.__index = UIManager

function UIManager.new()
    local self = setmetatable({}, UIManager)
    self.elements = {}
    return self
end

function UIManager:addElement(element)
    table.insert(self.elements, element)
end

function UIManager:draw()
    for _, element in ipairs(self.elements) do
        if element.draw then
            element:draw()
        end
    end
end

function UIManager:update()
    for _, element in ipairs(self.elements) do
        if element.update then
            element:update()
        end
    end
end

function UIManager:mousepressed(x, y, button)
    for _, element in ipairs(self.elements) do
        if element.mousepressed then
            element:mousepressed(x, y, button)
        end
    end
end

function UIManager:mousereleased(x, y, button)
    for _, element in ipairs(self.elements) do
        if element.mousereleased then
            element:mousereleased(x, y, button)
        end
    end
end

function UIManager:mousemoved(x, y)
    for _, element in ipairs(self.elements) do
        if element.mousemoved then
            element:mousemoved(x, y)
        end
    end
end

function UIManager:key(key_code, ctrl_pressed, shift_pressed)
    for _, element in ipairs(self.elements) do
        if element.key then
            element:key(key_code, ctrl_pressed, shift_pressed)
        end
    end
end

function UIManager:keyup(key_code, ctrl_pressed, shift_pressed)
    for _, element in ipairs(self.elements) do
        if element.keyup then
            element:keyup(key_code, ctrl_pressed, shift_pressed)
        end
    end
end

return UIManager