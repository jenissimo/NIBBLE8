local TabBar = {}

TabBar.x = 38
TabBar.tabWidth = 7 -- Width of each tab
TabBar.tabHeight = 7 -- Height of the tab bar
TabBar.plusButtonWidth = 7 -- Width of the plus button
TabBar.margin = 1 -- Margin between tabs and between tab and plus button
TabBar.tabColor = 1
TabBar.tabSelectedColor = 0
TabBar.textColor = 2
TabBar.textSelectedColor = 3

function TabBar:update(textEditor) end

function TabBar:draw(textEditor)
    local x = self.margin + self.x
    local y = 0

    -- Draw the tabs
    for i = 1, #textEditor.tabs do
        local tabColor = i == textEditor.currentTab and self.tabSelectedColor or
                          self.tabColor
        local textColor = i == textEditor.currentTab and self.textSelectedColor or
                           self.textColor

        rectfill(x + 1, y, self.tabWidth - 2, self.tabHeight, tabColor)
        line(x, y + self.tabHeight - 1, x, y + 1, tabColor)
        line(x + self.tabWidth - 1, y + self.tabHeight - 1,
             x + self.tabWidth - 1, y + 1, tabColor)
        if i <= 10 then
            print(tostring(i - 1), x + 2, y + 1, textColor)
        else
            print(chr(96 + i - 10), x + 2, y + 1, textColor)
        end

        -- Update x position for next tab
        x = x + self.tabWidth + self.margin
    end

    print("+", x + 2, y + 1, 1)
end

function TabBar:isMouseOverTab(textEditor, mouseX, mouseY, tabIndex)
    local x = self.margin + self.x + (tabIndex - 1) *
                  (self.tabWidth + self.margin)
    local y = 0 -- Assuming the Y position is always 0 for tabs
    local tabEndX = x + self.tabWidth

    return mouseX >= x and mouseX <= tabEndX and mouseY >= y and mouseY <= y +
               self.tabHeight
end

function TabBar:isMouseOverPlusButton(textEditor, mouseX, mouseY)
    local x = self.margin + self.x + #textEditor.tabs *
                  (self.tabWidth + self.margin)
    local y = 0 -- Assuming the Y position is always 0 for the plus button
    local buttonEndX = x + self.plusButtonWidth

    return
        mouseX >= x and mouseX <= buttonEndX and mouseY >= y and mouseY <= y +
            self.tabHeight
end

function TabBar:mousep(textEditor, x, y, button)
    if button == 1 then -- Assuming 1 is the left mouse button
        trace("TabBar:mousep")
        for i = 1, #textEditor.tabs do
            if self:isMouseOverTab(textEditor, x, y, i) then
                trace("Switch to tab", i)
                textEditor:switchToTab(i)
                return
            end
        end

        if self:isMouseOverPlusButton(textEditor, x, y) then
            trace("Create tab")
            textEditor:createTab({})
            textEditor:switchToTab(#textEditor.tabs)
        end
    end
end

return TabBar
