prototypes = {}
prototypes.defs = {}

function prototypes.def(self, data)
  local function merge(dst, src)
    for k, v in pairs(src) do
      if k ~= "name" then
        if (type(v) == "table") then
          local t = dst[k]
          if t == nil then
            t = {}
            dst[k] = t
          end
          merge(t, v)
        else
          dst[k] = v
        end
      end
    end
  end

  for name, prototype in pairs(data) do
    local def = self.defs[name]
    if def == nil then
      def = {}
      self.defs[name] = def;
    end

    merge(def, prototype)
  end
end

function prototypes.inherits(self)
  local function take_inherit(dst, src)
    for k, v in pairs(src) do
      if type(v) == "table" then
        local t = dst[k]
        if t == nil then
          t = {}
          dst[k] = t
        end
        take_inherit(t, v)
      else
        if dst[k] == nil then
          dst[k] = v
        end
      end
    end
  end

  for _, def in pairs(self.defs) do
    local b = def["base"]
    if b ~= nil then
      if type(b) ~= "table" then
        b = {b}
      end
      for _, name in ipairs(b) do
        if type(name) ~= "string" then
          error("Expected string. Prototype: " ..
            serpent.block(def, {comment=false}))
        end
        local base = self.defs[name]
        if base == nil then
          error("Base \"" .. name .. "\" not found. Prototype: " ..
            serpent.block(def, {comment=false}))
        end
        take_inherit(def, base)
      end
      def["base"] = nil
    end
  end
end

function prototypes:dump()
  print(serpent.block(self.defs, {comment=false}))
end
