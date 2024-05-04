local UIUtils = {}

function UIUtils.fancyPrint(text, x, y, c1, c2)
    print(text, x, y + 1, c2)
    print(text, x, y, c1)
end

function UIUtils.fancyRect(x, y, w, h, c1, c2, bg)
    if bg ~= nil then rectfill(x + 1, y + 1, w - 1, h - 1, bg) end
    line(x + 1, y, x + w - 1, y, c1) -- top
    line(x, y + 1, x, y + h - 1, c1) -- left
    line(x + w, y + 1, x + w, y + h - 1, c2) -- right
    line(x + 1, y + h, x + w - 1, y + h, c2) -- bottom
end

return UIUtils
