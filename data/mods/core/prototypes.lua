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
      value = 5
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

prototypes:def({
  gabe = {
    sprite_sheet = {
      image = "core/resources/gabe-idle-run.png",
      columns = 7,
      rows = 1
    },
    position = {x = 50, y = 50},
    health = {
      max = 400,
      value = 3
    }
  }
})

prototypes:def({
  tmp01 = { sprite = "core/resources/tmp/generic-rpg-tile01.png" },
  tmp02 = { sprite = "core/resources/tmp/generic-rpg-tile02.png" },
  tmp03 = { sprite = "core/resources/tmp/generic-rpg-tile03.png" },
  tmp04 = { sprite = "core/resources/tmp/generic-rpg-tile04.png" },
  tmp05 = { sprite = "core/resources/tmp/generic-rpg-tile05.png" },
  tmp06 = { sprite = "core/resources/tmp/generic-rpg-tile06.png" },
  tmp07 = { sprite = "core/resources/tmp/generic-rpg-tile07.png" },
  tmp08 = { sprite = "core/resources/tmp/generic-rpg-tile08.png" },
  tmp09 = { sprite = "core/resources/tmp/generic-rpg-tile09.png" },
  tmp10 = { sprite = "core/resources/tmp/generic-rpg-tile10.png" },
  tmp11 = { sprite = "core/resources/tmp/generic-rpg-tile11.png" },
  tmp12 = { sprite = "core/resources/tmp/generic-rpg-tile12.png" },
  tmp13 = { sprite = "core/resources/tmp/generic-rpg-tile13.png" },
  tmp14 = { sprite = "core/resources/tmp/generic-rpg-tile14.png" },
  tmp15 = { sprite = "core/resources/tmp/generic-rpg-tile15.png" },
  tmp16 = { sprite = "core/resources/tmp/generic-rpg-tile16.png" },
  tmp17 = { sprite = "core/resources/tmp/generic-rpg-tile17.png" },
  tmp18 = { sprite = "core/resources/tmp/generic-rpg-tile18.png" },
  tmp19 = { sprite = "core/resources/tmp/generic-rpg-tile19.png" },
  tmp20 = { sprite = "core/resources/tmp/generic-rpg-tile20.png" },
  tmp21 = { sprite = "core/resources/tmp/generic-rpg-tile21.png" },
  tmp22 = { sprite = "core/resources/tmp/generic-rpg-tile22.png" },
  tmp23 = { sprite = "core/resources/tmp/generic-rpg-tile23.png" },
  tmp24 = { sprite = "core/resources/tmp/generic-rpg-tile24.png" },
  tmp25 = { sprite = "core/resources/tmp/generic-rpg-tile25.png" },
  tmp26 = { sprite = "core/resources/tmp/generic-rpg-tile26.png" },
  tmp27 = { sprite = "core/resources/tmp/generic-rpg-tile27.png" },
  tmp28 = { sprite = "core/resources/tmp/generic-rpg-tile28.png" },
  tmp29 = { sprite = "core/resources/tmp/generic-rpg-tile29.png" },
  tmp30 = { sprite = "core/resources/tmp/generic-rpg-tile30.png" },
  tmp31 = { sprite = "core/resources/tmp/generic-rpg-tile31.png" },
  tmp32 = { sprite = "core/resources/tmp/generic-rpg-tile32.png" },
  tmp33 = { sprite = "core/resources/tmp/generic-rpg-tile33.png" },
  tmp34 = { sprite = "core/resources/tmp/generic-rpg-tile34.png" },
  tmp35 = { sprite = "core/resources/tmp/generic-rpg-tile35.png" },
  tmp36 = { sprite = "core/resources/tmp/generic-rpg-tile36.png" },
  tmp37 = { sprite = "core/resources/tmp/generic-rpg-tile37.png" },
  tmp38 = { sprite = "core/resources/tmp/generic-rpg-tile38.png" },
  tmp39 = { sprite = "core/resources/tmp/generic-rpg-tile39.png" },
  tmp40 = { sprite = "core/resources/tmp/generic-rpg-tile40.png" },
  tmp41 = { sprite = "core/resources/tmp/generic-rpg-tile41.png" },
  tmp42 = { sprite = "core/resources/tmp/generic-rpg-tile42.png" },
  tmp43 = { sprite = "core/resources/tmp/generic-rpg-tile43.png" },
  tmp44 = { sprite = "core/resources/tmp/generic-rpg-tile44.png" },
  tmp45 = { sprite = "core/resources/tmp/generic-rpg-tile45.png" },
  tmp46 = { sprite = "core/resources/tmp/generic-rpg-tile46.png" },
  tmp47 = { sprite = "core/resources/tmp/generic-rpg-tile47.png" },
  tmp48 = { sprite = "core/resources/tmp/generic-rpg-tile48.png" },
  tmp49 = { sprite = "core/resources/tmp/generic-rpg-tile49.png" },
  tmp50 = { sprite = "core/resources/tmp/generic-rpg-tile50.png" },
  tmp51 = { sprite = "core/resources/tmp/generic-rpg-tile51.png" },
  tmp52 = { sprite = "core/resources/tmp/generic-rpg-tile52.png" },
  tmp53 = { sprite = "core/resources/tmp/generic-rpg-tile53.png" },
  tmp54 = { sprite = "core/resources/tmp/generic-rpg-tile54.png" },
  tmp55 = { sprite = "core/resources/tmp/generic-rpg-tile55.png" },
  tmp56 = { sprite = "core/resources/tmp/generic-rpg-tile56.png" },
  tmp57 = { sprite = "core/resources/tmp/generic-rpg-tile57.png" },
  tmp58 = { sprite = "core/resources/tmp/generic-rpg-tile58.png" },
  tmp59 = { sprite = "core/resources/tmp/generic-rpg-tile59.png" },
  tmp60 = { sprite = "core/resources/tmp/generic-rpg-tile60.png" },
  tmp61 = { sprite = "core/resources/tmp/generic-rpg-tile61.png" },
  tmp62 = { sprite = "core/resources/tmp/generic-rpg-tile62.png" },
  tmp63 = { sprite = "core/resources/tmp/generic-rpg-tile63.png" },
  tmp64 = { sprite = "core/resources/tmp/generic-rpg-tile64.png" },
  tmp65 = { sprite = "core/resources/tmp/generic-rpg-tile65.png" },
  tmp66 = { sprite = "core/resources/tmp/generic-rpg-tile66.png" },
  tmp67 = { sprite = "core/resources/tmp/generic-rpg-tile67.png" },
  tmp68 = { sprite = "core/resources/tmp/generic-rpg-tile68.png" },
  tmp69 = { sprite = "core/resources/tmp/generic-rpg-tile69.png" },
  tmp70 = { sprite = "core/resources/tmp/generic-rpg-tile70.png" },
  tmp71 = { sprite = "core/resources/tmp/generic-rpg-tile71.png" },
  tmp72 = { sprite = "core/resources/tmp/generic-rpg-tree01.png" },
  tmp73 = { sprite = "core/resources/tmp/generic-rpg-tree02.png" },
  tmp74 = { sprite = "core/resources/tmp/generic-rpg-house-inn.png" },
  tmp75 = { sprite = "core/resources/tmp/generic-rpg-bridge.png" },
  tmp76 = { sprite = "core/resources/tmp/mani-idle-run.png" },
  tmp77 = { sprite = "core/resources/tmp/generic-rpg-vendor.png" },
  tmp78 = { sprite = "core/resources/tmp/generic-rpg-ui-inventario.png" },
  tmp79 = { sprite = "core/resources/tmp/generic-rpg-ui-text-box.png" },
})

prototypes:def({
  tstmap1 = {
    map = {
      tile_size = {x=16, y=16},
      chunk_size = {x=32, y=32},
    },
    tile_set = {
      "core/resources/tmp/generic-rpg-tile01.png",
      "core/resources/tmp/generic-rpg-anim-tile1.png",
      "core/resources/tmp/generic-rpg-tile71.png",
      "core/resources/tmp/generic-rpg-tile03.png",
      "core/resources/tmp/generic-rpg-tile04.png",
      "core/resources/tmp/generic-rpg-tile05.png",
    },
  }
})
