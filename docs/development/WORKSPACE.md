# Development workspace notes (mcRPC)

```
/data/projects/mcrpc      ← this repository
/data/projects/meshcore   ← MeshCore consumer
```

MeshCore depends on this tree via PlatformIO:

```ini
lib_deps = symlink://../mcrpc
```

See `/data/projects/README.md` for the daily workflow.
