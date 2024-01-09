local TabBar = {}

TabBar.x = 38
TabBar.tabWidth = 7
TabBar.tabHeight = 7
TabBar.plusButtonWidth = 7
TabBar.margin = 1
TabBar.tabColor = 1
TabBar.tabSelectedColor = 0
TabBar.textColor = 2
TabBar.textSelectedColor = 3
TabBar.scrollIndex = 1
TabBar.maxVisibleTabs = 14
TabBar.totalTabWidth = 0
TabBar.uiElements = {}
TabBar.dirty = true

function TabBar:update(textEditor) end

function TabBar:draw(textEditor)
    if self.dirty then self:invalidateUIElements(textEditor) end
    -- Draw the UI elements
    for _, element in ipairs(self.uiElements) do
        local x = element.x1 + self.x
        local y = element.y1

        if element.type == "scrollLeft" then
            print("<", x + 2, y + 1, self.tabColor)
        elseif element.type == "tab" then
            local tabColor = element.index == textEditor.currentTab and
                                 self.tabSelectedColor or self.tabColor
            local textColor = element.index == textEditor.currentTab and
                                  self.textSelectedColor or self.textColor
            local tabText =
                element.index <= 10 and tostring(element.index - 1) or
                    chr(96 + element.index - 10)

            rectfill(x + 1, y, self.tabWidth - 2, self.tabHeight, tabColor)
            line(x, y + self.tabHeight - 1, x, y + 1, tabColor)
            line(x + self.tabWidth - 1, y + self.tabHeight - 1,
                 x + self.tabWidth - 1, y + 1, tabColor)
            print(tabText, x + 2, y + 1, textColor)
        elseif element.type == "scrollRight" then
            print(">", x + 2, y + 1, self.tabColor)
        elseif element.type == "plusButton" then
            print("+", x + 2, y + 1, self.tabColor)
        end
    end
end

function TabBar:getVisibleTabsCount(textEditor)
    local visibleTabsCount = self.maxVisibleTabs

    -- Check if both scroll buttons are potentially needed
    if #textEditor.tabs > self.maxVisibleTabs then
        -- Reduce the visible tabs count by 1 when both scroll buttons are shown
        if self.scrollIndex > 1 then
            visibleTabsCount = visibleTabsCount - 1
        end
        if self.scrollIndex <= (#textEditor.tabs - self.maxVisibleTabs + 1) then
            visibleTabsCount = visibleTabsCount - 1
        end
    end

    return visibleTabsCount
end

function TabBar:scrollLeft(textEditor)
    self.scrollIndex = math.max(1, self.scrollIndex - 1)
end

function TabBar:scrollRight(textEditor)
    local visibleTabsCount = self:getVisibleTabsCount(textEditor)

    self.scrollIndex = math.min(#textEditor.tabs - visibleTabsCount + 1,
                                self.scrollIndex + 1)
end

function TabBar:updateTabSwitch(textEditor, tabIndex)
    self:invalidateUIElements(textEditor)
    local visibleTabsCount = self:getVisibleTabsCount(textEditor)
    -- Now, adjust TabBar.scrollIndex to ensure the tab is visible
    if tabIndex < self.scrollIndex then
        -- If the tab is to the left of the visible tabs
        self.scrollIndex = tabIndex
    elseif tabIndex >= TabBar.scrollIndex + visibleTabsCount then
        -- If the tab is to the right of the visible tabs
        self.scrollIndex = tabIndex - visibleTabsCount + 1
    end
end

function TabBar:invalidateUIElements(textEditor)
    self.uiElements = {} -- Reset the UI elements table

    local x = 0 -- Starting x position
    local y = 0 -- Starting y position is always 0 in this case
    local visibleTabsCount = self:getVisibleTabsCount(textEditor)

    -- Left scroll button
    if self.scrollIndex > 1 then
        table.insert(self.uiElements, {
            type = "scrollLeft",
            x1 = x,
            y1 = y,
            x2 = x + self.tabWidth,
            y2 = y + self.tabHeight
        })
        x = x + self.tabWidth + self.margin
    end

    -- Tabs
    local startIndex = self.scrollIndex
    local endIndex = math.min(startIndex + visibleTabsCount - 1,
                              #textEditor.tabs)

    for i = startIndex, endIndex do
        table.insert(self.uiElements, {
            type = "tab",
            index = i,
            x1 = x,
            y1 = y,
            x2 = x + self.tabWidth,
            y2 = y + self.tabHeight
        })
        x = x + self.tabWidth + self.margin
    end

    -- Right scroll button
    local showRightScroll = #textEditor.tabs > self.maxVisibleTabs and
                                self.scrollIndex <
                                (#textEditor.tabs - visibleTabsCount + 1)
    if showRightScroll then
        table.insert(self.uiElements, {
            type = "scrollRight",
            x1 = x,
            y1 = y,
            x2 = x + self.tabWidth,
            y2 = y + self.tabHeight
        })
        x = x + self.tabWidth + self.margin
    end

    -- Plus button
    table.insert(self.uiElements, {
        type = "plusButton",
        x1 = x,
        y1 = y,
        x2 = x + self.plusButtonWidth,
        y2 = y + self.tabHeight
    })
    x = x + self.tabWidth + self.margin

    -- Calculate the total width based on the uiElements
    local totalWidth = 0
    for _, element in ipairs(self.uiElements) do
        totalWidth = totalWidth + (element.x2 - element.x1) + self.margin
    end

    -- Subtract the last margin as it's not needed after the last element
    totalWidth = totalWidth - self.margin

    -- Set the total width and update the x position
    self.totalTabWidth = x - self.margin -- Assuming x is the position after the last element
    self.x = 160 - self.totalTabWidth

    if #textEditor.tabs <= self.maxVisibleTabs then
        self.scrollIndex = 1
    end

    self.dirty = false -- Reset the dirty flag after updating
end

function TabBar:mousep(textEditor, mouseX, mouseY, button)
    if button == 1 then -- Left mouse button
        for _, element in ipairs(self.uiElements) do
            local x1 = element.x1 + self.x
            local x2 = element.x2 + self.x
            local y1 = element.y1
            local y2 = element.y2

            if mouseX >= x1 and mouseX <= x2 and mouseY >= y1 and mouseY <= y2 then
                if element.type == "scrollLeft" then
                    trace("Scroll left")
                    self:scrollLeft(textEditor)
                    self.dirty = true
                elseif element.type == "scrollRight" then
                    trace("Scroll right")
                    self:scrollRight(textEditor)
                    self.dirty = true
                elseif element.type == "tab" then
                    textEditor:switchToTab(element.index, true)
                    self.dirty = true
                elseif element.type == "plusButton" then
                    textEditor:createTab({})
                    textEditor:switchToTab(#textEditor.tabs)
                    self.dirty = true
                end
                return
            end
        end
    end
end

return TabBar
