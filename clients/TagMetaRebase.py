import json
import sys

def main(base, input, output):
	with open(input, 'r') as f:
		fjson = json.load(f)
		for entry in fjson:
			entry["offset"] += base
	with open(output, 'w') as h:
		json.dump(fjson, h)

def usage():
	print("Usage : TagMetaRebase.py <base> <input tag file> <output tag file>")
		
if __name__ == "__main__":
	if len(sys.argv) != 4:
		usage()
		exit(0)
	main(sys.argv[1], sys.argv[2], sys.argv[3])