#!/usr/bin/env python
from pprint import pprint
from pathlib import Path
from libpressio import PressioCompressor
import numpy as np

# dataset_path = Path.home() / "git/datasets/hurricane/100x500x500/CLOUDf48.bin.f32"
# uncompressed_data = np.fromfile(dataset_path, dtype=np.float32)
# uncompressed_data = uncompressed_data.reshape(500, 500, 100)
uncompressed_data = np.random.rand(500, 500, 100)
decompressed_data = uncompressed_data.copy()

# load and configure the compressor
compressor = PressioCompressor.from_config({
    "compressor_id": "sz",
    "compressor_config": {
        "sz:error_bound_mode_str": "abs",
        "sz:abs_err_bound": 1e-6,
        "sz:metric": "size"
        }
    })

# compressor = PressioCompressor.from_config({
#     "compressor_id": "zfp",
#     "compressor_config": {
#         "zfp:metric": "size"
#         }
#     })

# compressor = PressioCompressor.from_config({
#     "compressor_id": "fpzip",
#     "compressor_config": {
#         "fpzip:metric": "size"
#         }
#     })

# print out some metadata
print(compressor.codec_id)
pprint(compressor.get_config())
# pprint(compressor.get_compile_config())


# preform compression and decompression
compressed = compressor.encode(uncompressed_data)
decompressed = compressor.decode(compressed, decompressed_data)

# print out some metrics collected during compression
pprint(compressor.get_metrics())