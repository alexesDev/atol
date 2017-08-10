{
  "targets": [{
    "target_name": "addon",
      "cflags": [
        "-std=c++11",
      ],
      "include_dirs": [
        "./atol",
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/addon.cc",
        "src/atol.cc"
      ]
  }]
}
