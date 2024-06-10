# Cluster IO Plugins ![Build Status](https://github.com/ManiVaultStudio/ClusterIO/actions/workflows/build.yml/badge.svg?branch=main)

Plugins to save cluster dataset to and read them from disk for the [ManiVault](https://github.com/ManiVaultStudio/core) visual analytics framework.

```bash
git clone git@github.com:ManiVaultStudio/ClusterIO.git
```
The exporter creates a `.txt` file in a addition to a binary file with some meta data, e.g. `file.txt`:
```cpp
file.bin
Num clusters: 42
Source data: ParentName
Num data points (source/parent): 234 
```

Binary layout:
```
Numer of parent dataset points (uint32_t)
Number of clusters (uint32_t)
Size of cluster sizes (size_t), cluster sizes (uint32_t)
Size of cluster colors (size_t), parent cluster colors (int32_t)
Size of cluster indices (size_t), cluster indices (uint32_t)
Size of cluster names (size_t), for each: size of cluster name (size_t), cluster name (chars)
Size of cluster IDs (size_t), for each: size of cluster ID (size_t), cluster ID (chars)
Size of parent dataset name (size_t), parent dataset name (chars)
```

## How to use
- In Manivault, exporters are opened by right-clicking on a data set in the data hierarchy, selecting the "Export" field and further chosing the desired exporter (`Cluster Exporter`).
- Either right-click an empty area in the data hierachy and select `Import` -> `Cluster Loader` or in the main menu, open `File` -> `Import data...` -> `Cluster Loader`.
    - You must select a parent data set for the loaded clusters which must have the same number of total points as the `Num data points` in the accompanying test file.
