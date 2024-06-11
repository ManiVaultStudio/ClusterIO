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
Number of clusters (uint32_t)
Numer of parent dataset points (uint32_t)
Size of cluster sizes (size_t), cluster sizes (uint32_t)
Bool whether this entry exists, size of cluster colors (size_t), parent cluster colors (int32_t)
Size of cluster indices (size_t), cluster indices (uint32_t)
Size of cluster names (size_t), for each: size of cluster name (size_t), cluster name (chars)
Size of parent dataset name (size_t), parent dataset name (chars)
```

## How to use
- In Manivault, exporters are opened by right-clicking on a data set in the data hierarchy, selecting the "Export" field and further chosing the desired exporter (`Cluster Exporter`).
- Either right-click an empty area in the data hierachy and select `Import` -> `Cluster Loader` or in the main menu, open `File` -> `Import data...` -> `Cluster Loader`.
    - You must select a parent data set for the loaded clusters which must have the same number of total points as the `Num data points` in the accompanying test file.

## Exporting cluster data from Python
Given a pandas data frame, you can create cluster data sets that the `Cluster Loader` is able to read. These functions group the occurences of all unique values of a given class and create cluster of each item with the respective item IDs:
```python
import pandas as pd
import numpy as np


# This assumes that the data frame contains an `id` column, which might be obtained with `df['id'] = df.index`
def clusterByClass(df: pd.DataFrame, classname: str) -> dict:
    grouped = df.groupby(classname)['id'].apply(list)
    grouped_dict = grouped.to_dict()
    return grouped_dict


def saveClusterAsBin(df: pd.DataFrame, classname: str, filename: str) -> None:
    grouped_dict = clusterByClass(df, classname)

    with open(filename, 'wb') as f:
        # Number of clusters (uint32_t)
        num_clusters = len(grouped_dict)
        f.write(num_clusters.to_bytes(4, byteorder='little', signed=False))
        # Numer of parent dataset points (uint32_t)
        num_points = len(df.index)
        f.write(num_points.to_bytes(4, byteorder='little', signed=False))
        # Size of cluster sizes (size_t), cluster sizes (uint32_t)
        f.write(num_clusters.to_bytes(8, byteorder='little', signed=False))
        for cluster_id, point_ids in grouped_dict.items():
            num_points_cluster = len(point_ids)
            f.write(num_points_cluster.to_bytes(4, byteorder='little', signed=False))
        # No colors
        colors = False
        f.write(colors.to_bytes(1, byteorder='little', signed=False))
        # Size of cluster indices (size_t), cluster indices (uint32_t)
        num_entries = sum(len(value) for value in grouped_dict.values())
        f.write(num_entries.to_bytes(8, byteorder='little', signed=False))
        for cluster_id, point_ids in grouped_dict.items():
            for point_id in point_ids:
                f.write(point_id.to_bytes(4, byteorder='little', signed=False))
        # Size of cluster names (size_t), for each: size of cluster name (size_t), cluster name (chars)
        f.write(num_clusters.to_bytes(8, byteorder='little', signed=False))
        for cluster_id, point_ids in grouped_dict.items():
            string_bytes = f"{cluster_id}".encode('utf-8')
            f.write(len(string_bytes).to_bytes(8, byteorder='little', signed=False))
            f.write(string_bytes)
        # Size of parent dataset name (size_t), parent dataset name (chars)
        name_bytes = "embedding".encode('utf-8')
        f.write(len(name_bytes).to_bytes(8, byteorder='little', signed=False))
        f.write(name_bytes)
```
> This assumes a 64-bit system. On 32-bit machines the `size_t` width will be 4 instead of 8
