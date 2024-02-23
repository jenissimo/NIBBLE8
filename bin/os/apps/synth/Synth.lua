local Synth = {}
Synth.__index = Synth

local UIManager = require("ui/UIManager")
local ArrowButton = require("ui/ArrowButton")
local Knob = require("os/apps/synth/Knob")
local Keyboard = require("os/apps/synth/Keyboard")
local WaveSelector = require("os/apps/synth/WaveSelector")
local KeyboardUtils = require("os/apps/synth/KeyboardUtils")
local HGroup = require("os/apps/synth/HGroup")
local ToggleSwitch = require("os/apps/synth/ToggleSwitch")

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

local knobCO
local knobRES

local highPassSwitch
local lowPassSwitch
local bandPassSwitch
local notchSwitch

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
    modsGroup = HGroup.new(self.x + 34, self.y + 4, "mods", 2, {5, 6, 5, 10},
                           uiManager)
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
    filterGroup = HGroup.new(self.x + 34, self.y + 47, "filters", 2,
                             {5, 6, 5, 10}, uiManager)

    knobCO = Knob.new(0, 64, 4, 0, 1, 0.5, 8, "co", self.invalidateFilter)
    knobRES = Knob.new(0, 64, 4, 0, 1, 0.5, 8, "res", self.invalidateFilter)
    highPassSwitch = ToggleSwitch.new(0, 0, 10, 10, false, "HP",
                                      self.invalidateFilter)
    lowPassSwitch = ToggleSwitch.new(0, 0, 10, 10, false, "LP",
                                     self.invalidateFilter)
    bandPassSwitch = ToggleSwitch.new(0, 0, 10, 10, false, "BP",
                                      self.invalidateFilter)
    notchSwitch = ToggleSwitch.new(0, 0, 10, 10, false, "NT",
                                   self.invalidateFilter)

    filterGroup:addElement(knobCO)
    filterGroup:addElement(knobRES)
    filterGroup:addElement(highPassSwitch)
    filterGroup:addElement(lowPassSwitch)
    filterGroup:addElement(bandPassSwitch)
    filterGroup:addElement(notchSwitch)
    uiManager:addElement(filterGroup)
end

function Synth.invalidateFilter()
    -- Define the bit positions for each filter type
    local LP_BIT = 1 -- 0001
    local BP_BIT = 2 -- 0010
    local HP_BIT = 4 -- 0100
    local NT_BIT = 8 -- 1000

    -- Initialize the mode variable
    local mode = 0

    -- Set the appropriate bits based on the toggle switch states
    --if highPassSwitch.state then mode = mode | HP_BIT end
    --if lowPassSwitch.state then mode = mode | LP_BIT end
    --if bandPassSwitch.state then mode = mode | BP_BIT end
    --if notchSwitch.state then mode = mode | NT_BIT end

    -- Call the update_filter function with the bit field mode
    update_filter(knobCO.value, knobRES.value, mode)
    trace("update_filter")

end

function Synth:initWave()
    waveGroup = HGroup.new(self.x + 4, self.y + 4, "wave", 2, {8, 7, 8, 7},
                           uiManager)
    waveSelector = WaveSelector.new(0, 0, Synth.updateConfig)
    waveGroup:addElement(waveSelector)
    uiManager:addElement(waveGroup)
end

function Synth.init() self:updateConfig() end

function Synth:update() uiManager:update() end

function Synth.updateConfig()
    trace("update config")
    keyboard.currentInstrument = waveSelector.selectedTab - 1
    trace("update_synth: " .. "osc: " .. (waveSelector.selectedTab - 1) ..
              ", a: " .. knobA.value .. ", d: " .. knobD.value .. ", s: " ..
              knobS.value .. ", r: " .. knobR.value, 1)
    update_synth(waveSelector.selectedTab - 1, knobA.value, knobD.value,
                 knobS.value, knobR.value, 1);
end

function Synth:draw()
    rectfill(self.x, self.y, 160, 112, 0)
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
    if key_code == KEYCODE.KEY_LEFTBRACKET then
        trace("left bracket")
        Synth.prevOctave()
    elseif key_code == KEYCODE.KEY_RIGHTBRACKET then
        Synth.nextOctave()
    end
end

return Synth
