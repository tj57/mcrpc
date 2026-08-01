# Development workspace notes (mcRPC)

Canonical sibling layout:

```
/data/projects/mcrpc      ← this repository
/data/projects/meshcore   ← MeshCore consumer
```

MeshCore depends on this tree via PlatformIO:

```ini
lib_deps = file://../mcrpc
```

See `/data/projects/README.md` for the daily workflow.
