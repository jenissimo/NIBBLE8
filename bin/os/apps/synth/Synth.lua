local Synth = {}
Synth.__index = Synth

local UIManager = require("ui/UIManager")
local ArrowButton = require("ui/ArrowButton")
local Knob = require("os/apps/synth/Knob")
local Keyboard = require("os/apps/synth/Keyboard")
local WaveSelector = require("os/apps/synth/WaveSelector")
local KeyboardUtils = require("os/apps/synth/KeyboardUtils")
local HGroup = require("os/apps/synth/HGroup")

local uiManager
local keyboard
local octaveLeftArrow
local octaveRightArrow

local waveGroup
local modsGroup
local filterGroup

local waveSelector

local knobA
local knobD
local knobS
local knobR

local knobDE
local knobBU
local knobCO
local knobLP
local knobNO
local knobRE
local knobDA

function Synth.new(x, y)
    local self = setmetatable({}, Synth)

    self.x = x
    self.y = y

    uiManager = UIManager.new()

    self:initADSR()
    self:initFilters()
    self:initWave()

    keyboard = Keyboard.new(41, 89, 2)
    octaveLeftArrow = ArrowButton.new(keyboard.x - 4, self.y + keyboard.y,
                                      "left", self.prevOctave)
    octaveRightArrow = ArrowButton.new(keyboard.x + keyboard.width + 4,
                                       self.y + keyboard.y, "right",
                                       self.nextOctave)

    uiManager:addElement(keyboard)
    uiManager:addElement(octaveLeftArrow)
    uiManager:addElement(octaveRightArrow)
    return self
end

function Synth:initADSR()
    modsGroup = HGroup.new(self.x+34, self.y+4, "mods", 2, {5,6,5,10}, uiManager)
    knobA = Knob.new(0, 0, 4, 0, 1, 0, 8, "a", self.updateConfig)
    knobD = Knob.new(0, 0, 4, 0, 1, 0.5, 8, "d", self.updateConfig)
    knobS = Knob.new(0, 0, 4, 0, 1, 0.5, 8, "s", self.updateConfig)
    knobR = Knob.new(0, 0, 4, 0, 1, 0.5, 8, "r", self.updateConfig)
    modsGroup:addElement(knobA)
    modsGroup:addElement(knobD)
    modsGroup:addElement(knobS)
    modsGroup:addElement(knobR)
    uiManager:addElement(modsGroup)
end

function Synth:initFilters()
    filterGroup = HGroup.new(self.x+34, self.y+47, "filters", 2, {5,6,5,10}, uiManager)

    knobCO = Knob.new(0, 64, 4, 0, 1, 0, 8, "co",function() update_filter(0, knobCO.value) end)
    knobLP = Knob.new(0, 64, 4, 0, 1, 0, 8, "lp",function() update_filter(1, knobLP.value) end)
    knobDE = Knob.new(0, 64, 4, 0, 1, 0, 8, "de",function() update_filter(2, knobDE.value) end)
    knobBU = Knob.new(0, 64, 4, 0, 1, 0, 8, "bu",function() update_filter(3, knobBU.value) end)
    knobNO = Knob.new(0, 64, 4, 0, 1, 0, 8, "no",function() update_filter(4, knobNO.value) end)
    knobRE = Knob.new(0, 64, 4, 0, 1, 0, 8, "re",function() update_filter(5, knobRE.value) end)
    knobDA = Knob.new(0, 64, 4, 0, 1, 0, 8, "da",function() update_filter(6, knobDA.value) end)

    filterGroup:addElement(knobCO)
    filterGroup:addElement(knobLP)
    filterGroup:addElement(knobDE)
    filterGroup:addElement(knobBU)
    filterGroup:addElement(knobNO)
    filterGroup:addElement(knobRE)
    filterGroup:addElement(knobDA)
    uiManager:addElement(filterGroup)
end

function Synth:initWave()
    waveGroup = HGroup.new(self.x+4, self.y+4, "wave", 2, {8,7,8,7}, uiManager)
    waveSelector = WaveSelector.new(0, 0, Synth.updateConfig)
    waveGroup:addElement(waveSelector)
    uiManager:addElement(waveGroup)
end

function Synth.init()
     slef:updateConfig()
end

function Synth:update() uiManager:update() end

function Synth.updateConfig()
    trace("update config")
    trace("update_synth: ".."osc: "..(waveSelector.selectedTab - 1)..", a: "..knobA.value..", d: "..knobD.value..", s: "..knobS.value..", r: "..knobR.value, 1)
    update_synth(waveSelector.selectedTab - 1, knobA.value, knobD.value, knobS.value, knobR.value, 1);
    --update_synth(waveSelector.selectedTab, knobA.value, knobD.value, knobS.value, knobR.value, 1);
    --void nibble_api_update_filter(uint8_t filter, double value);
end

function Synth:draw()
    --cls()
    rectfill(self.x, self.y, 160, 112, 0)
    -- rectfill(self.x, self.y, 160, 112, 0)
    --self:drawGroup(4, 4, 28, 68, "wave")
    --self:drawGroup(3 + 54, 4, 44, 26, "mods")
    uiManager:draw()
    self:drawPanel()
end

function Synth:drawGroup(x, y, width, height, name)
    -- draw lines
    rect(self.x + x, self.y + y, width, height, 3)

    local prx = self.x + x + (width - (#name * 4 - 1)) / 2 + 1
    local pry = self.y + y - 2
    local padding = 4

    rectfill(prx - padding, pry - 1, #name * 4 + padding * 2 - 1, 7, 0)
    print(name, prx, pry, 3)
end

function Synth:drawPanel()
    local status = ""
    rectfill(self.x, 113, 160, 7, 2)
    status = status .. "octave offset: " .. keyboard.currentOctave
    if keyboard.pressed_key then
        status = status .. " key: " ..
                     KeyboardUtils.keyNumberToNoteName(keyboard.pressed_key,
                                                       keyboard.currentOctave)
    end
    print(status, 1, 114, 0)
end

function Synth.prevOctave()
    keyboard.currentOctave = keyboard.currentOctave - 1
    if keyboard.currentOctave <= keyboard.minOctave then
        keyboard.currentOctave = keyboard.minOctave
        octaveLeftArrow.enabled = false
    else
        octaveLeftArrow.enabled = true
        octaveRightArrow.enabled = true
    end
end
function Synth.nextOctave()
    keyboard.currentOctave = keyboard.currentOctave + 1
    if keyboard.currentOctave >= keyboard.maxOctave then
        keyboard.currentOctave = keyboard.maxOctave
        octaveRightArrow.enabled = false
    else
        octaveRightArrow.enabled = true
        octaveLeftArrow.enabled = true
    end

end

function Synth:mousep(x, y, button) uiManager:mousepressed(x, y, button) end

function Synth:mouser(x, y, button) uiManager:mousereleased(x, y, button) end

function Synth:mousem(x, y) uiManager:mousemoved(x, y) end

function Synth:key(key_code, ctrl_pressed, shift_pressed)
    uiManager:key(key_code, ctrl_pressed, shift_pressed)
end

function Synth:keyup(key_code, ctrl_pressed, shift_pressed)
    uiManager:keyup(key_code, ctrl_pressed, shift_pressed)
end

return Synth
