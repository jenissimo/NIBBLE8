local Intro = {}

function Intro:init(duration, cnt)
    cls()
    self.duration = duration
    self.cnt = cnt
    self.start_t = t()
    self.active = true
end

function Intro:draw()
    local dt = t() - self.start_t
    -- little delay at start
    if dt < 0.2 then return end

    if dt > self.duration then
        cls()
        self.active = false
        return
    end

    palt(0, false)
    for i = 1, self.cnt do
        local y = flr(rnd(120))
        local c = flr(rnd(4)) + 1
        if dt >= 0.6*self.duration then c = 0 end
        line(0, y, 160, y, c)
    end
    palt(0, true)
    -- print(str(dt),8,8,3)
end

return Intro
