import os
import sys
from nntool.api import NNGraph
from PIL import Image
import numpy as np

model_path = sys.argv[1]
image_path = sys.argv[2]
ext = os.path.splitext(model_path)[1]
print(ext)
G = NNGraph.load_graph(model_path, load_quantization=ext == ".tflite")

input_image = np.array(Image.open(image_path)) / 128 - 1
if ext != ".json":
	G.adjust_order()
	G.fusions("scaled_match_group")
	stats = None
	G.quantize(
		stats,
		graph_options={
			"use_ne16": True,
			"hwc": True
		}
	)
#G.draw(quant_labels=True)
out = G.execute([input_image], quantize=True)
pred_class = np.argmax(out[-1][0])

print(f"Predicted class: {pred_class} with confidence: {out[-1][0].flatten()[pred_class]}")
