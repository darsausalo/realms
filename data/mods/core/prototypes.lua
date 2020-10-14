require("prototypes/base")

prototypes:def({
  ["@infantry"] = {
    health = {
      max = 100
    },
    position = {
      x = 50,
      y = -100
    }
  },
  soldier = {
    base = "@infantry",
  },
  soldier2 = {
    base = {"@infantry", "@with_sprite"},
    health = {
      max = 400,
      value = 2
    },
    timer = {duration = 1},
    tag = "enemy"
  }
})

prototypes:def({
  ["@infantry"] = {
    position = {
      x = 150
    }
  }
})
