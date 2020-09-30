require("prototypes/base")

prototypes:def({
  ["@infantry"] = {
    name = "",
    health = {
      max = 100
    },
    position = {
      x = 100,
      y = 200
    }
  },
  soldier = {
    base = {"@infantry", "@with_sprite"}
  },
  soldier2 = {
    base = "@infantry",
    health = {
      max = 400,
      value = 1
    },
    tag = "enemy"
  }
})

prototypes:def({
  ["@infantry"] = {
    position = {
      x = 300
    }
  }
})
