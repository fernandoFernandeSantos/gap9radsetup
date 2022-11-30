from nntool.api import NNGraph
from nntool.stats.activation_ranges_collector import ActivationRangesCollector

import os
import argparse
import argcomplete
from tqdm import tqdm
from PIL import Image
from glob import glob
import numpy as np
from preprocessing import preprocess
import pickle

def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='test_accuracy')

	parser.add_argument('model_path', type=str,
						help='path to model (quantized and prepared for nntool inference)')
	parser.add_argument('--val_dataset', type=str, required=True,
						help='where to store statistics')
	return parser

def main():
	parser = create_parser()
	argcomplete.autocomplete(parser)
	args = parser.parse_args()

	G = NNGraph.load_graph(args.model_path)

	with open('synset.txt', 'r') as f:
		labels = [l.split()[0] for l in f]

	count = 0
	correct = 0
	for class_dir in tqdm(glob(args.val_dataset + "/*")):
		class_synset = os.path.split(class_dir)[-1]
		label = labels.index(class_synset)
		correct_class = 0
		count_class = 0
		for image in tqdm(glob(class_dir + "/*")):
			try:
				input_tensor = np.array(Image.open(image).convert(mode="RGB").resize((224, 224))).transpose(2, 0, 1)
				input_tensor = preprocess(input_tensor)
				input_tensor = input_tensor.transpose((1, 2, 0))

				out = G.execute([input_tensor], dequantize=True)

				pred_class = np.argmax(out[-1][0])
				correct_class += pred_class == label
				count_class += 1
			except Exception as e:
				print(f"problem with image {image}... skipping")

		correct += correct_class
		count += count_class
		print(f"Accuracy class {label}:\t\t\t{correct_class / count_class * 100:.2f}% ({correct / count * 100:.2f}%) ")

	with open(args.stats_path, "wb") as f:
		pickle.dump(stats_collector.stats, f)


if __name__ == '__main__':
	main()
