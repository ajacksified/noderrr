{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "addon.cc", "r3.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "ldflags": [
          "-Wl,-rpath,/usr/local/lib",
          "<!@(pkg-config r3 --libs-only-L)"
        ],
        "libraries": [
          "<!@(pkg-config r3 --libs-only-l)"
        ]
      }
    }
  ]
}
